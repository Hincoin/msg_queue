#include <functional>
#include <iostream>
#include <thread>
#include <future>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <cmath>
#include <memory>
class message_queue
{
private:
    
    std::thread executing;
    std::queue<std::function<void()>> job_queue;
    std::mutex retrieval_lock;
    std::mutex queue_lock;
    sem_t* job_sem; // semaphore controlling the count of elements in the job_queue
    bool is_done;
    
public:
    
    
    message_queue()
    {
        is_done = false;
        job_sem = sem_open("/job_sem",O_CREAT,0666,0);
        
        if(job_sem == SEM_FAILED)
        {
            perror("FAILED AGAIN!");
            abort();
        }
        executing = std::thread([&]()
                                {
                                    while(!is_done) // while this message queue is still alive
                                    {
                                       
                                    
                                        sem_wait(job_sem); // wait for the semaphore to have a value > 0
                                        queue_lock.lock();
                                        auto nxt_func = job_queue.front();
                                        
                                        job_queue.pop();
                                        queue_lock.unlock();
                                        
                                        
                                        printf("Launching!\n");
                                       // auto returnVal = std::async(std::launch::async, nxt_func);
                                       // std::thread(nxt_func).detach();
                                        //std::async(std::launch::async,nxt_func);
                                        std::thread(nxt_func).detach();
                                    }
                                    
                                });
        
    }
 

 
    template<typename F,typename ...Arguments>
    auto add_message(F ftn, Arguments&&... args)  -> std::future<decltype(ftn(std::forward<Arguments>(args)...))>
    {
		
      //  queue_lock.lock();l
        queue_lock.lock();
     
       // std::unique_lock<std::mutex> unique_queue_lock(queue_lock);
        //auto p = std::promise<decltype(ftn(std::forward<Arguments>(args)...))>();
        auto p = std::make_shared<std::promise<decltype(ftn(std::forward<Arguments>(args)...))>>();
        auto ret = p->get_future();
        
        
        
                // auto return_future = p.get_future().share(); // return a std::shared_future<>
        //queue_lock.lock();
        job_queue.push([=]()
                       {
                           try
                           {
                               printf("Printing out the addreses of values\n");
                               printf("p: %p\nftn: %p\n",&p,&ftn);
                        //    set_value(p,ftn,std::forward<Arguments>(args)...);
                               
                               set_value(*p,ftn,args...);
                           }catch(...)
                           {
                               p->set_exception(std::current_exception());
                           }
                           
                       });
        
        sem_post(job_sem);
        

        //   ftn(std::forward<Arguments>(args)...);
       queue_lock.unlock();
       
        return ret;
        
        
    }
    
    template<typename Future,typename Function,typename... Arguments>
    void set_value(std::promise<Future>& p, Function& f, Arguments&&... args)
    {
        p.set_value(f(std::forward<Arguments>(args)...));
        
    }
    template<typename Function,typename... Arguments>
    void set_value(std::promise<void>& p, Function& f, Arguments&&... args)
    {
        f(std::forward<Arguments>(args)...);
        p.set_value();
        
    }
    size_t jobsLeft()
    {
        queue_lock.lock();
        size_t queue_size = job_queue.size();
        queue_lock.unlock();
        
        return queue_size;
        
    }
    
    ~message_queue()
    {
        //is_done = true;
		//	added.clear();
        //		return_values.clear();
        queue_lock.lock();
        job_queue.push([&](){
            
            
            is_done=true;
            
        });
        sem_post(job_sem);
        queue_lock.unlock();
        sem_close(job_sem);

        sem_unlink("/job_sem");
        executing.join();
    }
    
    
    
};




double  get_distance(int x1, int y1, int x2, int y2)
{
    printf("DIST\n");
	double x = abs(pow(x1-x2,2) + pow(y1-y2,2));
    printf("Done g_dist!\n");
    return x;
}
bool isPowerOfTwo(int x)
{
	bool x1 =  (x & (x-1)) == 0;
    printf("Done isPowerOfTwo\n");
    return x1;
}

std::vector<int> getFirstSevenFib()
{
    
    printf("Executing desired function!!\n");
    
	std::vector<int> nums;
	nums.push_back(0);
	nums.push_back(1);
	nums.push_back(1);
	nums.push_back(2);
	nums.push_back(3);
   
    for(int i =0 ; i < 1000; ++i)
    {
        nums.push_back(i* 12);
    }
    
    for(int i =0 ; i < 100; ++i)
        nums.insert(nums.begin()+356+i,37);
    
	nums.push_back(8);

    std::sort(nums.begin(),nums.end());
    std::random_shuffle(nums.begin(), nums.end());
    
    size_t size = nums.size()-1;
    for(int x = 2; x < size; ++x)
    {
        nums.insert(std::find(nums.begin(), nums.end(), 3),16);
        
    }
   
    for(int x = 0; x < 2000; ++x)
    {
        std::random_shuffle(nums.begin(), nums.end());
    }
    
  //  std::sort(nums.begin(),nums.end());
  
	printf("Done pushing back!");
    
	return nums;
}

bool isEven(int x)
{
	return (x & 1) == 0;
}

double complexMath(int x1, int x2)
{
    if(x2 <= 0)
        return 4.0;
    if(x1 <= 0)
        return 2.2;
       
    double d = x1 * x2 * x2 * x2 * x2 *x2 + 1 + 122324 - 121 / 12 + 23213 * 4.0 - (complexMath(x1-10,x2-5));
    printf("Done complex\n");
    return d;
}


int getRandomInt()
{
	return 4;
}


template<typename Future,typename Function>
auto takeFuture(Future f1,Function ftn) -> std::future<decltype(ftn(f1.get()))>
{
    
    
    auto lambda = [=](Future ff)
    {
        ftn(ff.get());
    };
    return std::async(std::launch::async,lambda,std::move(f1));
    
}
int main()
{
    
    printf("Concurrent: %i\n",std::thread::hardware_concurrency());
	message_queue mQueue;

    
    auto powerOfTow = mQueue.add_message(isPowerOfTwo,64);
    auto isEvenFut = mQueue.add_message(isEven, 1500);
    
    auto getFib = mQueue.add_message(getFirstSevenFib);
    auto workLambda = [](std::vector<int> vec){
        
        std::cout << "Look at my vector!\n";
        for(const auto& rp : vec)
        {
            std::cout << rp << "  ";
        }
        std::cout << "\n";
        
        
        
    };
    auto next = takeFuture(std::move(getFib),workLambda);

    auto complx = mQueue.add_message(complexMath, 22,5);
    auto nxt2 = takeFuture(std::move(complx),[](double d){std::cout << "LOOOK AT THIS HARD MATH: " << d << "\n";});
    
    auto rd = mQueue.add_message(getRandomInt);
    auto nxt3 = takeFuture(std::move(rd),[](int d){std::cout << "RANDOM WHACKY MATH: " << d << "\n";});

   
   
    
	return 0;
}