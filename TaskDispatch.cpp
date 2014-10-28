#include <assert.h>

#include "TaskDispatch.hpp"

static TaskDispatch* s_instance = nullptr;

TaskDispatch::TaskDispatch( size_t workers )
    : m_exit( false )
    , m_jobs( 0 )
{
    assert( !s_instance );
    s_instance = this;

    m_workers.reserve( workers );
    for( int i=0; i<workers; i++ )
    {
        m_workers.emplace_back( [this]{ Worker(); } );
    }
}

TaskDispatch::~TaskDispatch()
{
    m_exit = true;
    m_cvWork.notify_all();

    for( auto& worker : m_workers )
    {
        worker.join();
    }

    assert( s_instance );
    s_instance = nullptr;
}

void TaskDispatch::Queue( const std::function<void(void)>& f )
{
    std::lock_guard<std::mutex> lock( s_instance->m_queueLock );
    s_instance->m_queue.emplace( f );
    s_instance->m_cvWork.notify_one();
}

void TaskDispatch::Queue( std::function<void(void)>&& f )
{
    std::lock_guard<std::mutex> lock( s_instance->m_queueLock );
    s_instance->m_queue.emplace( std::move( f ) );
    s_instance->m_cvWork.notify_one();
}

void TaskDispatch::Sync()
{
  std::unique_lock<std::mutex> lock( s_instance->m_queueLock );
  s_instance->m_cvJobs.wait( lock, []{ return s_instance->m_queue.empty() && s_instance->m_jobs == 0; } );
}

void TaskDispatch::Worker()
{
    for(;;)
    {
        std::unique_lock<std::mutex> lock( m_queueLock );
        m_cvWork.wait( lock, [this]{ return !m_queue.empty() || m_exit; } );
        if( m_exit ) return;
        auto f = m_queue.front();
        m_queue.pop();
        m_jobs++;
        lock.unlock();
        f();
        lock.lock();
        m_jobs--;
        lock.unlock();
        m_cvJobs.notify_all();
    }
}
