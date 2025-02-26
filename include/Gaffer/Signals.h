//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2021, Cinesite VFX Ltd. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//
//      * Redistributions of source code must retain the above
//        copyright notice, this list of conditions and the following
//        disclaimer.
//
//      * Redistributions in binary form must reproduce the above
//        copyright notice, this list of conditions and the following
//        disclaimer in the documentation and/or other materials provided with
//        the distribution.
//
//      * Neither the name of John Haddon nor the names of
//        any other contributors to this software may be used to endorse or
//        promote products derived from this software without specific prior
//        written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Gaffer/Private/SlotBase.h"

#include "boost/callable_traits/return_type.hpp"
#include "boost/compressed_pair.hpp"
#include "boost/noncopyable.hpp"

#include <memory>
#include <type_traits>
#include <vector>

namespace Gaffer::Signals
{

/// Returned by `Signal::connect()`, and used to disconnect and/or block the
/// slot.
class Connection
{
	public :

		Connection() = default;
		Connection( const Connection &other ) = default;

		/// When a connection is blocked, the corresponding slot
		/// will not be called when the signal is emitted.
		void setBlocked( bool blocked );
		bool getBlocked() const;

		/// Removes the connection from the signal, and frees the slot.
		void disconnect();
		/// Returns true if the connection has not been disconnected yet.
		bool connected() const;

		Connection &operator=( const Connection &rhs ) = default;

	private :

		template<typename Signature, typename Combiner>
		friend class Signal;

		Connection( const Private::SlotBase::Ptr &slot );

		Private::SlotBase::Ptr m_slot;

};

/// Calls all slots in order, returning the result from the last
/// slot. If not slots are connected, returns a default-constructed `T`.
template<typename T>
struct DefaultCombiner;

/// Primary signal template. Example declaration :
///
/// ```
/// // A signal taking a string and returning an int.
/// using MySignal = Signal<int( const string &)>;
/// ```
///
/// See specialisation below for full details.
///
/// > Caution : Signals are not thread-safe. Limited support for concurrent
/// > emission (but not connection/disconnection) is currently provided to
/// > support legacy code, but it will be removed in future. Do not become
/// > dependent on it!
template<typename Signature, typename Combiner = DefaultCombiner<boost::callable_traits::return_type_t<Signature>>>
class Signal;

/// Specialisation that implements the primary template
/// defined above.
template<typename Result, typename... Args, typename Combiner>
class Signal<Result( Args... ), Combiner> : boost::noncopyable
{

	public :

		Signal( const Combiner &combiner = Combiner() );
		~Signal();

		/// Adds a slot that will be called when the signal
		/// is emitted by `operator()`. Slots are called in
		/// the order of connection, so this slot will be
		/// called after all previously connected slots.
		template<typename SlotFunctor>
		Connection connect( const SlotFunctor &slot );

		/// As for `connect()`, but adding `slot` to the front
		/// of the list of slots, so that it will be called
		/// _before_ previously connected slots.
		template<typename SlotFunctor>
		Connection connectFront( const SlotFunctor &slot );

		/// Disconnects all connected slots. Not recommended, because
		/// it allows the disconnection of slots belonging to others.
		void disconnectAllSlots();

		/// Returns the number of currently connected slots.
		/// Complexity : linear in the number of slots.
		size_t numSlots() const;
		/// Returns true if any slot is connected, false otherwise.
		/// Complexity : constant.
		bool empty() const;

		/// Emits the signal. This calls the connected slots and returns their
		/// combined result as defined by the Combiner template argument.
		Result operator() ( Args... args ) const;

		/// Used to present the results of each slot to the Combiner. Dereferencing
		/// the iterator calls the slot and returns the result. Only really public
		/// to allow access from the Python bindings.
		class SlotCallIterator;

		/// Compatibility with `boost::bind`.
		using result_type = Result;

	private :

		template<typename SlotFunctor>
		Connection connectInternal( const SlotFunctor &slot, bool front );

		Private::SlotBase::Ptr &lastSlot();
		const Private::SlotBase::Ptr &lastSlot() const;
		const Combiner &combiner() const;

		// Type derived from SlotBase, holding a FunctionType
		// object to be called when the signal is emitted.
		struct Slot;
		// Value type obtained by dereferencing SlotCallIterator. `void` is not
		// a valid `value_type` for an iterator, so we use a fake bool in this
		// special case.
		using SlotCallIteratorValueType = std::conditional_t<std::is_void_v<Result>, bool, Result>;
		// Type used to pass arguments to SlotCallIterator.
		using ArgsTuple = std::tuple<Args...>;

		Private::SlotBase::Ptr m_firstSlot;
		/// The combiner will often have no data members, and using
		/// `compressed_pair` means that in this case it won't contribute
		/// anything to `sizeof( Signal )`.
		/// \todo In c++20 this can be simplified with `[[no_unique_address]]`
		/// and we can remove the `lastSlot()` and `combiner()` accessors.
		boost::compressed_pair<Private::SlotBase::Ptr, Combiner> m_lastSlotAndCombiner;

};

/// Provides RAII-style connection management. When the ScopedConnection class
/// goes out of scope, it automatically disconnects the slot.
class ScopedConnection : public Connection
{

	public :

		ScopedConnection() = default;
		ScopedConnection( const Connection &connection );
		/// Move constructor, which transfers ownership from an existing
		/// ScopedConnection (which will subsequently be empty).
		ScopedConnection( ScopedConnection &&scopedConnection );
		/// Disconnects the slot.
		~ScopedConnection();
		/// Disconnects the current connection and assigns a new one.
		ScopedConnection &operator=( const Connection &connection );
		/// Disconnects the current connection and takes ownership of
		/// the connection held by `scopedConnection` (which will
		/// subsequently be empty).
		ScopedConnection &operator=( ScopedConnection &&scopedConnection );

};

/// The BlockedConnection class allows connections to be blocked and unblocked
/// in an exception-safe manner.
class BlockedConnection : boost::noncopyable
{

	public :

		/// Calls `connection.setBlocked( true )` if `block` is true, otherwise
		/// does nothing.
		BlockedConnection( Signals::Connection &connection, bool block = true );
		/// Restores the connection's blocking to its previous state.
		~BlockedConnection();

	private :

		Signals::Connection *m_connection;
		bool m_previouslyBlocked;

};

/// Equivalent to the DefaultCombiner, except that exceptions thrown from slots
/// are caught and reported via `IECore::MessageHandler`. This is useful for
/// situations where an error in a slot should not affect the calling of other
/// slots, or the emitter of the signal.
template<typename T>
struct CatchingCombiner;

/// Utility base class for objects which connect member functions to signals,
/// and therefore need to disconnect automatically when they are destroyed.
///
/// > Note : Connections _must_ be made via `boost::bind()` for them to be
/// > discovered and disconnected automatically. For example :
/// >
/// > `signal.connect( boost::bind( &TrackableSubclass::methodName, this ) )`
///
/// \todo Perhaps we should add a protected `track( const Connection &connection )`
/// method that could be used to track any sort of connection? Then we could replace
/// our usage of `boost::bind()` with `std::bind()`. Or perhaps `Signal::connect()`
/// should take multiple arguments and do the binding for you, so it naturally gets
/// to introspect the arguments looking for Trackables?
class Trackable : boost::noncopyable
{

	public :

		virtual ~Trackable();

	protected :

		void disconnectTrackedConnections();

	private :

		friend void GafferModule::bindSignals();

		template<typename Signature, typename Combiner>
		friend class Signal;

		template<typename SlotFunctor>
		static void trackConnection( const SlotFunctor &slotFunctor, const Connection &connection );

		struct TrackableVisitor;

		// Connections are held by a `unique_ptr<vector>` and initialised on
		// demand instead of storing `vector` directly. This reduces `sizeof(
		// Trackable )` and is a win for the common case (for Node and Plug
		// subclasses) where `m_connections` is never needed.
		mutable std::unique_ptr<std::vector<Connection>> m_connections;

};

} // namespace Gaffer::Signals

#include "Gaffer/Signals.inl"
