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

#include "IECore/MessageHandler.h"

#include "boost/function.hpp"
#include "boost/iterator/iterator_facade.hpp"
#include "boost/visit_each.hpp"

#include <optional>

namespace Gaffer::Signals
{

//////////////////////////////////////////////////////////////////////////
// Connection
//////////////////////////////////////////////////////////////////////////

inline Connection::Connection( const Private::SlotBase::Ptr &slot )
	:	m_slot( slot )
{
}

inline void Connection::setBlocked( bool blocked )
{
	if( m_slot )
	{
		m_slot->blocked = blocked;
	}
}

inline bool Connection::getBlocked() const
{
	return m_slot && m_slot->blocked;
};

inline void Connection::disconnect()
{
	if( !m_slot )
	{
		return;
	}
	m_slot->disconnect();
	m_slot = nullptr;
}

inline bool Connection::connected() const
{
	return m_slot && m_slot->previous;
}

//////////////////////////////////////////////////////////////////////////
// DefaultCombiner
//////////////////////////////////////////////////////////////////////////

template<typename T>
struct DefaultCombiner
{

	template<typename InputIterator>
	T operator()( InputIterator first, InputIterator last ) const
	{
		if constexpr( std::is_void_v<T> )
		{
			while( first != last )
			{
				*first;
				++first;
			}
		}
		else
		{
			T r = T();
			while( first != last )
			{
				r = *first;
				++first;
			}
			return r;
		}
	}

};

//////////////////////////////////////////////////////////////////////////
// Signal
//////////////////////////////////////////////////////////////////////////

template<typename Result, typename... Args, typename Combiner>
Signal<Result( Args... ), Combiner>::Signal( const Combiner &combiner )
	:	m_lastSlotAndCombiner( nullptr, combiner )
{
}

template<typename Result, typename... Args, typename Combiner>
Signal<Result( Args... ), Combiner>::~Signal()
{
	disconnectAllSlots();
}

template<typename Result, typename... Args, typename Combiner>
template<typename SlotFunctor>
Connection Signal<Result( Args... ), Combiner>::connect( const SlotFunctor &slot )
{
	return connectInternal( slot, /* front = */ false );
}

template<typename Result, typename... Args, typename Combiner>
template<typename SlotFunctor>
Connection Signal<Result( Args... ), Combiner>::connectFront( const SlotFunctor &slot )
{
	return connectInternal( slot, /* front = */ true );
}

template<typename Result, typename... Args, typename Combiner>
template<typename SlotFunctor>
Connection Signal<Result( Args... ), Combiner>::connectInternal( const SlotFunctor &slot, bool front )
{
	if( !lastSlot() )
	{
		assert( !m_firstSlot );
		lastSlot() = new Slot( m_firstSlot );
	}

	Private::SlotBase::Ptr s = new Slot(
		front ? m_firstSlot : *(lastSlot()->previous), slot
	);

	const Connection result = Connection( s );
	Trackable::trackConnection( slot, result );
	return result;
}

template<typename Result, typename... Args, typename Combiner>
Result Signal<Result( Args... ), Combiner>::operator() ( Args... args ) const
{
	ArgsTuple argsTuple( args... ); /// \todo : Capture by reference? Or forward_as_tuple?
	return combiner()(
		SlotCallIterator( m_firstSlot, argsTuple ),
		SlotCallIterator( nullptr, argsTuple )
	);
}

template<typename Result, typename... Args, typename Combiner>
void Signal<Result( Args... ), Combiner>::disconnectAllSlots()
{
	while( m_firstSlot != lastSlot() )
	{
		m_firstSlot->disconnect();
	}
	assert( m_firstSlot == lastSlot() );
};

template<typename Result, typename... Args, typename Combiner>
size_t Signal<Result( Args... ), Combiner>::numSlots() const
{
	size_t result = 0;
	const Private::SlotBase *s = m_firstSlot.get();
	while( s != lastSlot().get() )
	{
		result++;
		s = s->next.get();
	}
	return result;
};

template<typename Result, typename... Args, typename Combiner>
bool Signal<Result( Args... ), Combiner>::empty() const
{
	return m_firstSlot == lastSlot();
}

template<typename Result, typename... Args, typename Combiner>
Private::SlotBase::Ptr &Signal<Result( Args... ), Combiner>::lastSlot()
{
	return m_lastSlotAndCombiner.first();
}

template<typename Result, typename... Args, typename Combiner>
const Private::SlotBase::Ptr &Signal<Result( Args... ), Combiner>::lastSlot() const
{
	return m_lastSlotAndCombiner.first();
}

template<typename Result, typename... Args, typename Combiner>
const Combiner &Signal<Result( Args... ), Combiner>::combiner() const
{
	return m_lastSlotAndCombiner.second();
}

//////////////////////////////////////////////////////////////////////////
// Signal::Slot
//////////////////////////////////////////////////////////////////////////

template<typename Result, typename... Args, typename Combiner>
struct Signal<Result( Args... ), Combiner>::Slot : public Private::SlotBase
{

	using FunctionType = std::function<Result( Args... )>;

	Slot( Private::SlotBase::Ptr &previous, const FunctionType &function = FunctionType() )
		: 	SlotBase( previous ), function( function )
	{
	}

	void disconnect() override
	{
		// `SlotBase::disconnect()` might (or might not) destroy us.
		// Extend our lifetime to the end of this function.
		const Ptr lifePreserver( this );
		const bool wasConnected = previous;
		Private::SlotBase::disconnect();
		if( wasConnected && !calling )
		{
			// Clear our function, and any resources that are bound inside it.
			// Note : this can cause arbitrary code to run, including the
			// destruction of ScopedConnections for this slot, causing reentrant
			// calls to `this->disconnect()`. We use `wasConnected` to protect
			// against the double-clear this could otherwise cause.
			function = nullptr;
		}
	}

	Result operator()( Args... args )
	{
		CallScope callScope( *this );
		return function( args... );
	}

	struct CallScope : private boost::noncopyable
	{
		CallScope( Slot &slot )
			:	slot( slot )
		{
			// Slot can't be called if not connected
			assert( slot.previous );
			slot.calling = true;
		}
		~CallScope()
		{
			slot.calling = false;
			if( !slot.previous )
			{
				// Slot was disconnected during call, and we couldn't
				// clear the function while it was being called. Clear
				// it now instead.
				slot.function = nullptr;
			}
		}
		Slot &slot;
	};

	FunctionType function;

};

//////////////////////////////////////////////////////////////////////////
// Signal::SlotCallIterator
//////////////////////////////////////////////////////////////////////////

template<typename Result, typename... Args, typename Combiner>
class Signal<Result( Args... ), Combiner>::SlotCallIterator : public boost::iterator_facade<
	SlotCallIterator,
	SlotCallIteratorValueType,
	boost::single_pass_traversal_tag
>
{

	public :

		SlotCallIterator( const Private::SlotBase::Ptr &slot, const ArgsTuple &args )
			:	m_slot( slot ), m_args( args )
		{
			skipBlocked();
		}

	private :

		friend class boost::iterator_core_access;

		SlotCallIteratorValueType &dereference() const
		{
			if( !m_value )
			{
				if constexpr( std::is_void_v<Result> )
				{
					std::apply( static_cast<Slot &>( *m_slot ), m_args );
					m_value = true;
				}
				else
				{
					m_value = std::apply( static_cast<Slot &>( *m_slot ), m_args );
				}
			}
			return *m_value;
		}

		void increment()
		{
			assert( !atEnd() );
			m_slot = m_slot->next;
			m_value.reset();
			skipBlocked();
		}

		bool atEnd() const
		{
			return !m_slot || !m_slot->next;
		}

		bool equal( const SlotCallIterator &other ) const
		{
			if( atEnd() )
			{
				return other.atEnd();
			}
			else
			{
				return m_slot == other.m_slot;
			}
		}

		void skipBlocked()
		{
			while( !atEnd() && static_cast<Slot *>( m_slot.get() )->blocked )
			{
				m_slot = m_slot->next;
			}
		}

		Private::SlotBase::Ptr m_slot;
		const ArgsTuple &m_args;

		mutable std::optional<SlotCallIteratorValueType> m_value;

};

//////////////////////////////////////////////////////////////////////////
// CatchingCombiner
//////////////////////////////////////////////////////////////////////////

template<typename T>
struct CatchingCombiner
{

	template<typename InputIterator>
	T operator()( InputIterator first, InputIterator last ) const
	{
		if constexpr( std::is_void_v<T> )
		{
			while( first != last )
			{
				try
				{
					*first;
				}
				catch( const std::exception &e )
				{
					IECore::msg( IECore::Msg::Error, "Emitting signal", e.what() );
				}
				catch( ... )
				{
					IECore::msg( IECore::Msg::Error, "Emitting signal", "Unknown error" );
				}
				++first;
			}
		}
		else
		{
			T r = T();
			while( first != last )
			{
				try
				{
					r = *first;
				}
				catch( const std::exception &e )
				{
					IECore::msg( IECore::Msg::Error, "Emitting signal", e.what() );
				}
				catch( ... )
				{
					IECore::msg( IECore::Msg::Error, "Emitting signal", "Unknown error" );
				}
				++first;
			}
			return r;
		}
	}

};

//////////////////////////////////////////////////////////////////////////
// Trackable
//////////////////////////////////////////////////////////////////////////

inline Trackable::~Trackable()
{
	disconnectTrackedConnections();
}

struct Trackable::TrackableVisitor
{

	TrackableVisitor( const Connection &connection ) : connection( connection ) {}

	template<typename T>
	void operator()( T &x ) const
	{
		if constexpr( std::is_base_of_v<Trackable, T> )
		{
			const Trackable &trackable = static_cast<const Trackable &>( x );
			if( !trackable.m_connections )
			{
				trackable.m_connections = std::make_unique<std::vector<Connection>>();
			}
			trackable.m_connections->push_back( connection );
		}
		else if constexpr( std::is_pointer_v<T> )
		{
			this->operator()( *x );
		}
	}

	const Connection &connection;

};

template<typename SlotFunctor>
void Trackable::trackConnection( const SlotFunctor &slotFunctor, const Connection &connection )
{
	/// \todo Should we purge expired connections periodically?

	// Rather than call `boost::visit_each()` explicitly, we must bring it into
	// scope with `using` and then just call `visit_each()`. This allows
	// argument-dependent-lookup to find specialised versions of `visit_each()`
	// for types in other namespaces.
	using boost::visit_each;

	// `visit_each` allows us to discover Trackable objects inside callables
	// such as `boost::bind()`. Note that `std::bind()` is _not_ supported.
	TrackableVisitor visitor( connection );
	visit_each(
		visitor,
		slotFunctor,
		0
	);
}

inline void Trackable::disconnectTrackedConnections()
{
	if( m_connections )
	{
		for( auto &c : *m_connections )
		{
			c.disconnect();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// ScopedConnection
//////////////////////////////////////////////////////////////////////////

inline ScopedConnection::ScopedConnection( const Connection &connection )
	:	Connection( connection )
{
}

inline ScopedConnection::ScopedConnection( ScopedConnection &&scopedConnection )
	:	Connection( scopedConnection )
{
	scopedConnection.Connection::operator=( Connection() );
}

inline ScopedConnection::~ScopedConnection()
{
	disconnect();
}

inline ScopedConnection &ScopedConnection::operator=( const Connection &connection )
{
	disconnect();
	Connection::operator=( connection );
	return *this;
}

inline ScopedConnection &ScopedConnection::operator=( ScopedConnection &&scopedConnection )
{
	disconnect();
	Connection::operator=( scopedConnection );
	scopedConnection.Connection::operator=( Connection() );
	return *this;
}

//////////////////////////////////////////////////////////////////////////
// BlockedConnection
//////////////////////////////////////////////////////////////////////////

inline BlockedConnection::BlockedConnection( Signals::Connection &connection, bool block )
	:	m_connection( nullptr ), m_previouslyBlocked( false )
{
	if( block && connection.connected() )
	{
		m_connection = &connection;
		m_previouslyBlocked = m_connection->getBlocked();
		m_connection->setBlocked( true );
	}
}

inline BlockedConnection::~BlockedConnection()
{
	if( m_connection )
	{
		m_connection->setBlocked( m_previouslyBlocked );
	}
}

} // namespace Gaffer::Signals
