//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2016, Image Engine Design Inc. All rights reserved.
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

#include "Gaffer/Monitor.h"

#include "IECore/MurmurHash.h"
#include "IECore/RefCounted.h"

#include "boost/unordered_map.hpp"
#include "boost/unordered_set.hpp"

#include "tbb/enumerable_thread_specific.h"

#include <map>
#include <vector>

namespace Gaffer
{

IE_CORE_FORWARDDECLARE( GraphComponent )
IE_CORE_FORWARDDECLARE( Plug )
IE_CORE_FORWARDDECLARE( Context )

/// A monitor which collects statistics about
/// what contexts plugs are evaluated in.
class GAFFER_API ContextMonitor : public Monitor
{

	public :

		/// Statistics are only collected for the root and its
		/// descendants.
		explicit ContextMonitor( const GraphComponent *root = nullptr );
		~ContextMonitor() override;

		IE_CORE_DECLAREMEMBERPTR( ContextMonitor )

		struct GAFFER_API Statistics
		{

			using CountingMap = boost::unordered_map<IECore::MurmurHash, size_t>;

			size_t numUniqueContexts() const;
			std::vector<IECore::InternedString> variableNames() const;
			size_t numUniqueValues( IECore::InternedString variableName ) const;
			/// Maps from the `Context::variableHash()` for each unique value to
			/// the number of times that value appeared.
			const CountingMap &variableHashes( IECore::InternedString variableName ) const;

			Statistics & operator += ( const Context *rhs );
			Statistics & operator += ( const Statistics &rhs );

			bool operator == ( const Statistics &rhs );
			bool operator != ( const Statistics &rhs );

			private :

				using ContextSet = boost::unordered_set<IECore::MurmurHash>;
				using VariableMap = std::map<IECore::InternedString, CountingMap>;

				ContextSet m_contexts;
				VariableMap m_variables;

		};

		using StatisticsMap = boost::unordered_map<ConstPlugPtr, Statistics>;

		const StatisticsMap &allStatistics() const;
		const Statistics &plugStatistics( const Plug *plug ) const;
		const Statistics &combinedStatistics() const;

	protected :

		void processStarted( const Process *process ) override;
		void processFinished( const Process *process ) override;

	private :

		const GraphComponent *m_root;

		// For performance reasons we accumulate our statistics into
		// thread local storage while computations are running.
		struct ThreadData
		{
			// Stores the per-plug statistics captured by this thread.
			StatisticsMap statistics;
		};

		tbb::enumerable_thread_specific<ThreadData, tbb::cache_aligned_allocator<ThreadData>, tbb::ets_key_per_instance> m_threadData;

		// Then when we want to query it, we collate it into m_statistics.
		void collate() const;
		mutable StatisticsMap m_statistics;
		mutable Statistics m_combinedStatistics;

};

IE_CORE_DECLAREPTR( ContextMonitor )

} // namespace Gaffer
