#ifndef __UnboundedQueue_hpp__
#define __UnboundedQueue_hpp__
#include <iostream>
#include <list>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <type_traits>
#include <assert.h>

//������� ������������ �������������� ��� ������ ��������� ������ �++11. ������� ������������� �����.
//������ �� ��� TUnboundedQueue - Unbounded - ��� �����.
//���-������ ������ TBoundedQueue. TUnboundedQueue ����� ��������� � ���������� ������. ���� ��������� ����.
//TUnboundedQueue_ - ������� ������� TUnboundedQueue ��� ����������� �� ��������, �������� ����� � ������� ���������, ����������� ����� �������, �� ������ ������ ��������� ������ � ����������, ��������� ���-�� ������, ������������� �� ����� ����������.


struct TUnboundedQueueAbstractException : std::exception
{};

struct TUnboundedQueueTimeoutException : TUnboundedQueueAbstractException
{};

struct TUnboundedQueueClosedException : TUnboundedQueueAbstractException
{};

template<class TData>
class TUnboundedQueueForNonThrowMovable
{
    static_assert(::std::is_nothrow_move_constructible<TData>::value, "TData must be nonthrow movable type.");
    static_assert(!::std::is_array<TData>::value, "TData must not be c-array.");

public:
    typedef TData value_type;
    typedef ::std::list<TData> TDataList;

private:
    typedef ::std::list<TData> TQueue;
    typedef ::std::unique_lock<::std::mutex> TLock;
    TQueue _queue;
    mutable ::std::mutex _lockQueue;
    ::std::condition_variable _pushToQueue;
    bool _closed;

    TUnboundedQueueForNonThrowMovable(const TUnboundedQueueForNonThrowMovable&) = delete;
    TUnboundedQueueForNonThrowMovable(TUnboundedQueueForNonThrowMovable&&) = delete;
    TUnboundedQueueForNonThrowMovable& operator=(const TUnboundedQueueForNonThrowMovable&) = delete;
    TUnboundedQueueForNonThrowMovable& operator=(TUnboundedQueueForNonThrowMovable&&) = delete;
public:
    TUnboundedQueueForNonThrowMovable(void):_closed(false){}

    void close(void)	//�������� ���� ������ �� wait_and_pop ���������� TUnboundedQueueClosedException.
    {					//����� �� ���� ���� ��� ��������� ������� ���������� ������ ���� ����� �� ������� � �������� wait_and_pop. �� ���� ����� ���.
        TLock lockerQueue(this->_lockQueue);
        this->_closed = true;
        this->_pushToQueue.notify_all();
    }

    void push_front(TData data)
    //to do:
    {//����� �� ���� �������� ������� push_*, ����������� ������ push_all_*. ��� ���� �� ����������� �� ���������� ��������� ��������� � ������� �� ���. ���� ������� ������ �� �������� ��������� ��������� �� ������� *pop_all.
    //���� ��� �������� ��� ������ push_all_*, �� �� ��������, ��� ��� ����� �������� ������ ������ � ����� �� ��������� ������ ������� � ���������� ��������� � �������.
    //� ����� ����������, ���� ������� ��� �������, ����������� � �������� ���� ������� �\�� �������, �� ��� ��������� �� ���������� �� ���������� is_nothrow_move_constructible �� ��� TData.
    //� ��� �� � ���� ������� ��������� ������ ����� �������� � ������� ��� ������������� ������������ ������� � shared_ ��� unique_ pointer.
    //�� ���� ��� �� �� ���.
        TLock lockerQueue(this->_lockQueue);
        assert(!this->_closed);
        if(this->_closed)
            return;
        this->_queue.push_front(std::move(data));
        this->_pushToQueue.notify_one();
    }

    void push_back(TData data)
    {
        TLock lockerQueue(this->_lockQueue);
        assert(!this->_closed);
        if(this->_closed)
            return;
        this->_queue.push_back(std::move(data));
        this->_pushToQueue.notify_one();
    }

    bool emptyUnstable(void) const//Not recommended to use.
    {
        TLock lockerQueue(this->_lockQueue);
        return this->_queue.empty();
    }

    size_t sizeUnstable(void) const//Not recommended to use.
    {
        TLock lockerQueue(this->_lockQueue);
        return this->_queue.size();
    }

    void clear(void)//Not recommended to use.
    {
        TLock lockerQueue(this->_lockQueue);
        this->_queue.clear();
    }

    TData wait_and_pop(void)//throw(TUnboundedQueueClosedException)
    {
        return this->wait_and_pop_internal(
            [this](TLock& lockerQueue){this->waitForDataPushToQueue(lockerQueue);},
            [this](){return this->moveDataFromQueueFront();}
        );
    }

    TDataList wait_and_pop_all(void)//throw(TUnboundedQueueClosedException)
    {
        return this->wait_and_pop_internal(
            [this](TLock& lockerQueue){this->waitForDataPushToQueue(lockerQueue);},
            [this](){return this->moveAllDataFromQueue();}
        );
    }

    template<class Rep, class Period>
    TData wait_and_pop(const std::chrono::duration<Rep, Period> WaitDuration) //throw(TUnboundedQueueTimeoutException, TUnboundedQueueClosedException)
    {
        return this->wait_and_pop_internal(
            [this, WaitDuration](TLock& lockerQueue){this->waitForDataPushToQueueOrTimeout(lockerQueue,WaitDuration);},
            [this](){return this->moveDataFromQueueFront();}
        );
    }

    template<class Rep, class Period>
    TDataList wait_and_pop_all(const std::chrono::duration<Rep, Period> WaitDuration) //throw(TUnboundedQueueTimeoutException, TUnboundedQueueClosedException)
    {
        return this->wait_and_pop_internal(
            [this, WaitDuration](TLock& lockerQueue){this->waitForDataPushToQueueOrTimeout(lockerQueue,WaitDuration);},
            [this](){return this->moveAllDataFromQueue();}
        );
    }

    TData pop(void) //throw(TUnboundedQueueTimeoutException, TUnboundedQueueClosedException)
    {
        return this->wait_and_pop_internal(
            [this](TLock& lockerQueue){this->rightNowThrowTimeoutExceptionIfNoExistDataAtQueue(lockerQueue);},
            [this](){return this->moveDataFromQueueFront();}
        );
    }

    TDataList pop_all(void) //throw(TUnboundedQueueTimeoutException, TUnboundedQueueClosedException)
    //to do:
    {//����� ���������� ��� �� ������� *pop_all ���������� ������, �� ����� *pop_all ������� ������ ������ ���������� TUnboundedQueueTimeoutException - ����� �� ���� ������ ���������� ������ ������. ���� ��� �� ���.
        return this->wait_and_pop_internal(
            [this](TLock& lockerQueue){this->rightNowThrowTimeoutExceptionIfNoExistDataAtQueue(lockerQueue);},
            [this](){return this->moveAllDataFromQueue();}
        );
    }

    TData waitCentiTimeoutPop(const unsigned int WaitTime) //throw(TUnboundedQueueTimeoutException, TUnboundedQueueClosedException)
    {//������������ �������, �������.
        return this->wait_and_pop<int, std::centi>(std::chrono::duration<int, std::centi>(WaitTime));
    }

private:
    template<class TWaitFunctor, class TMoveFunctor>
    typename ::std::result_of<TMoveFunctor()>::type wait_and_pop_internal(const TWaitFunctor& WaitDataInQueue, const TMoveFunctor& MoveDataFromQueue)
    {
        TLock lockerQueue(this->_lockQueue);
        WaitDataInQueue(lockerQueue);
        const struct TExceptionSafety {
            TQueue& _queue;
            ::std::condition_variable& _pushToQueue;
            ~TExceptionSafety(){
                if(!this->_queue.empty())
                    this->_pushToQueue.notify_one();
            }
        } exceptionSafety = {_queue, _pushToQueue};//Use BOOST_SCOPE_EXIT, Luke!
        assert(!this->_queue.empty());
        return MoveDataFromQueue();
    }
    
    bool testPushAndThrowIfClossed(void) const
    {
        if(this->_closed)
            throw TUnboundedQueueClosedException();
        return !this->_queue.empty();
    }
    
    TData moveDataFromQueueFront(void)
    {
        TData result(::std::move(this->_queue.front()));
        this->_queue.pop_front();
        return result;
    }
    
    TDataList moveAllDataFromQueue(void)
    {
        TDataList result;
        result.splice(result.begin(), this->_queue);
        return result;
    }
    
    void waitForDataPushToQueue(TLock& lockerQueue)
    {
        this->_pushToQueue.wait(lockerQueue, [this](){return this->testPushAndThrowIfClossed();});
    }
    
    template< class Rep, class Period>
    void waitForDataPushToQueueOrTimeout(TLock& lockerQueue, const std::chrono::duration<Rep, Period> WaitDuration)
    {
        if(!this->_pushToQueue.wait_for(lockerQueue, WaitDuration, [this](){return this->testPushAndThrowIfClossed();}))
            throw TUnboundedQueueTimeoutException();
    }
    
    void rightNowThrowTimeoutExceptionIfNoExistDataAtQueue(TLock& lockerQueue) const
    {
        if(!this->testPushAndThrowIfClossed())
            throw TUnboundedQueueTimeoutException();
    }
};

template<class TData>
using TUnboundedQueueForUniquePtr = TUnboundedQueueForNonThrowMovable<std::unique_ptr<TData> >;
template<class TData>
using TUnboundedQueueForSharedPtr = TUnboundedQueueForNonThrowMovable<std::shared_ptr<const TData> >;
template<class TData>
using TUnboundedQueue = TUnboundedQueueForUniquePtr<TData>;//������� ������������ ��� ����.

/*
//��� ����� � ����� �����:
int main() {
    cout<<"ko"<<endl;
    {
        //TUnboundedQueueForNonThrowMovableOnly<int>::value_type == int
        TUnboundedQueueForNonThrowMovable<int> queue;
        int a = 10;
        queue.push(std::move(a));
        queue.push(45);
        int b = queue.wait_and_pop();
        cout<<b<<endl;
        assert(b==10);
        b = queue.wait_and_pop();
        cout<<b<<endl;
        assert(b==45);
    }
    {
        //TUnboundedQueueForSharedPtr<int>::value_type == std::shared_ptr<const int>
        TUnboundedQueueForSharedPtr<int> queueSharedPtr;
        auto a = std::make_shared<const int>(45);
        assert(a);
        queueSharedPtr.push(std::move(a));
        assert(!a);
        auto b = queueSharedPtr.wait_and_pop();//std::shared_ptr<const int>
        assert(b);
        cout<<*b<<endl;
        assert(*b==45);
        std::shared_ptr<const int> c = std::make_shared<const int>(46);
        assert(c);
        queueSharedPtr.push(std::move(c));
        assert(!c);
        b = queueSharedPtr.wait_and_pop();
        assert(b);
        cout<<*b<<endl;
        assert(*b==46);
        std::shared_ptr<const int> d(new int(47));
        assert(d);
        queueSharedPtr.push(std::move(d));
        assert(!d);
        b = queueSharedPtr.wait_and_pop();
        assert(b);
        cout<<*b<<endl;
        assert(*b==47);
        queueSharedPtr.push(std::make_shared<const int>(11));
        b = queueSharedPtr.wait_and_pop();
        assert(b);
        cout<<*b<<endl;
        assert(*b==11);
    }
    {
        //TUnboundedQueue<int>::value_type == std::unique_ptr<int>
        TUnboundedQueue<int> queueUniquePtr;
        //auto a = std::make_unique<int>(45);//C++1y only
        std::unique_ptr<int> a(new int(45));
        assert(a);
        queueUniquePtr.push(std::move(a));//queueUniquePtr.push(std::make_unique<int>(45));//C++1y only
        assert(!a);
        auto b = queueUniquePtr.wait_and_pop();//std::unique_ptr<int>
        assert(b);
        cout<<*b<<endl;
        assert(*b==45);
    }
    cout<<"ko"<<endl;
    return 0;
}*/

template<class TData>
class TUnboundedQueue_
{
    static_assert(!::std::is_array<TData>::value, "TData must not be c-array.");
    
public:
    typedef TData value_type;
    typedef ::std::list<TData> TDataContainer;

private:
    typedef ::std::list<TData> TQueue;
    typedef ::std::unique_lock<::std::mutex> TLock;
    TQueue _queue;
    mutable ::std::mutex _lockQueue;
    ::std::condition_variable _pushToQueue;
    bool _closed;

    TUnboundedQueue_(const TUnboundedQueue_&) = delete;
    TUnboundedQueue_(TUnboundedQueue_&&) = delete;
    TUnboundedQueue_& operator=(const TUnboundedQueue_&) = delete;
    TUnboundedQueue_& operator=(TUnboundedQueue_&&) = delete;
public:
    TUnboundedQueue_(void):_closed(false){}

    void close(void)	//�������� ���� ������ �� wait_and_pop_all ���������� TUnboundedQueueClosedException.
    {					//����� �� ���� ���� ��� ��������� ������� ���������� ������ ���� ����� �� ������� � �������� wait_and_pop_all. �� ���� ����� ���.
        TLock lockerQueue(this->_lockQueue);
        this->_closed = true;
        this->_pushToQueue.notify_all();
    }
    
    void push_front(const TData& data){
      this->push_front_all({data});
    }
    void push_front(TData&& data){
      this->push_front_all({::std::move(data)});
    }
    void push_back(const TData& data){
      this->push_back_all({data});
    }
    void push_back(TData&& data){
      this->push_back_all({::std::move(data)});
    }
    
    void push_front_all(TDataContainer& data){
      this->push_front_all(::std::move(data));
    }
    void push_front_all(TDataContainer&& data)
    {
        assert(data.get_allocator()==this->_queue.get_allocator());
        assert(&data!=&(this->_queue));
        if(data.empty())
          return;
        TLock lockerQueue(this->_lockQueue);
        assert(!this->_closed);
        if(this->_closed)
            return;
        this->_queue.splice(this->_queue.begin(), data);
        this->_pushToQueue.notify_one();
    }
    
    void push_back_all(TDataContainer& data){
      this->push_back_all(::std::move(data));
    }
    void push_back_all(TDataContainer&& data)
    {
        assert(data.get_allocator()==this->_queue.get_allocator());
        assert(&data!=&(this->_queue));
        if(data.empty())
          return;
        TLock lockerQueue(this->_lockQueue);
        assert(!this->_closed);
        if(this->_closed)
            return;
        this->_queue.splice(this->_queue.end(), data);
        this->_pushToQueue.notify_one();
    }
    
    bool emptyUnstable(void) const//Not recommended to use.
    {
        TLock lockerQueue(this->_lockQueue);
        return this->_queue.empty();
    }
    
    size_t sizeUnstable(void) const//Not recommended to use.
    {
        TLock lockerQueue(this->_lockQueue);
        return this->_queue.size();
    }
    
    void clear(void)//Not recommended to use.
    {
        TLock lockerQueue(this->_lockQueue);
        this->_queue.clear();
    }
    
    TDataContainer wait_and_pop_all(void)//throw(TUnboundedQueueClosedException)
    {
        return this->wait_and_pop_internal(
            [this](TLock& lockerQueue){this->waitForDataPushToQueue(lockerQueue);},
            [this](){return this->moveAllDataFromQueue();}
        );
    }
    
    template<class Rep, class Period>
    TDataContainer wait_and_pop_all(const std::chrono::duration<Rep, Period> WaitDuration) //throw(TUnboundedQueueTimeoutException, TUnboundedQueueClosedException)
    {
        return this->wait_and_pop_internal(
            [this, WaitDuration](TLock& lockerQueue){this->waitForDataPushToQueueOrTimeout(lockerQueue, WaitDuration);},
            [this](){return this->moveAllDataFromQueue();}
        );
    }
    
    TDataContainer pop_all(void) //throw(TUnboundedQueueTimeoutException, TUnboundedQueueClosedException)
    {
        return this->wait_and_pop_internal(
            [this](TLock& lockerQueue){this->rightNowThrowTimeoutExceptionIfNoExistDataAtQueue(lockerQueue);},
            [this](){return this->moveAllDataFromQueue();}
        );
    }
    
    TDataContainer pop_all_or_empty(void) //throw(TUnboundedQueueClosedException)
    {
      try{
        return this->pop_all();
      }catch(TUnboundedQueueTimeoutException&){
        return TDataContainer();
      };
    }
    
private:
    template<class TWaitFunctor, class TMoveFunctor>
    typename ::std::result_of<TMoveFunctor()>::type wait_and_pop_internal(const TWaitFunctor& WaitDataInQueue, const TMoveFunctor& MoveDataFromQueue)
    {
        TLock lockerQueue(this->_lockQueue);
        WaitDataInQueue(lockerQueue);
        const struct TExceptionSafety {
            TQueue& _queue;
            ::std::condition_variable& _pushToQueue;
            ~TExceptionSafety(){
                if(!this->_queue.empty())
                    this->_pushToQueue.notify_one();
            }
        } exceptionSafety = {_queue, _pushToQueue};//Use BOOST_SCOPE_EXIT, Luke!
        assert(!this->_queue.empty());
        return MoveDataFromQueue();
    }
    
    bool testPushAndThrowIfClossed(void) const
    {
        if(this->_closed)
            throw TUnboundedQueueClosedException();
        return !this->_queue.empty();
    }
    
    TDataContainer moveAllDataFromQueue(void)
    {
        TDataContainer result;
        result.splice(result.begin(), this->_queue);
        return result;
    }
    
    void waitForDataPushToQueue(TLock& lockerQueue)
    {
        this->_pushToQueue.wait(lockerQueue, [this](){return this->testPushAndThrowIfClossed();});
    }
    
    template< class Rep, class Period>
    void waitForDataPushToQueueOrTimeout(TLock& lockerQueue, const std::chrono::duration<Rep, Period> WaitDuration)
    {
        if(!this->_pushToQueue.wait_for(lockerQueue, WaitDuration, [this](){return this->testPushAndThrowIfClossed();}))
            throw TUnboundedQueueTimeoutException();
    }
    
    void rightNowThrowTimeoutExceptionIfNoExistDataAtQueue(TLock& lockerQueue) const
    {
        if(!this->testPushAndThrowIfClossed())
            throw TUnboundedQueueTimeoutException();
    }
};
template<class TData>
using TUnboundedQueue_ForUniquePtr = TUnboundedQueue_<std::unique_ptr<TData> >;
template<class TData>
using TUnboundedQueue_ForSharedPtr = TUnboundedQueue_<std::shared_ptr<const TData> >;
/*
//��� ����� � ����� �����:
int main()
    {
        //TUnboundedQueue_<int>::value_type == int
        TUnboundedQueue_<int> queue;
        typedef TUnboundedQueue_<int>::TDataContainer TDataContainer;
        int a = 10;
        queue.push_back_all({a,45});
        auto b = queue.wait_and_pop_all();
        ::std::cout<<b.front()<<b.back()<<::std::endl;
        assert(b==TDataContainer({a,45}));
    }
*/
#endif
