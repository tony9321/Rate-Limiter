/*func req: 
1. cli reach the endpoint
2. cli begin able to get a feed back
3. configure the params

non func req: 
aviabilty 
fault torelrance

data modeling:
user
server

api: allow(clientId, timeStamp)

scale: single server */

#include <bits/stdc++.h>
using namespace std;

class RateLimiter
{
	struct bucket
	{
		int limit;
		int window;
		deque<long long> q;
		Bucket(int l=0, int w=0):limit(l), window(w){}
	};
	unordered_map<string, Bucket> mp;
	int defLimit, defWindow;	

	mutex mp_m;
	int defLimit;
	long long defWindow;

	shared_ptr<Bucket> getBucket(const string&id)
	{
		lock_guard<mutex> lg(mp_m);
		auto it=mp.find(id);
		if(it!=mp.end()) return it->second;

		// not already in the map 
		auto b=make_shared<Bucket> (defLimit, defWindow);
		mp[id]=b;
		return b;
	}
public:
	RateLimiter(int limit, int window)
	{
		defLimit=limit;
		defWindow=window;
	}

	bool allow(const string& clientId, long long ts)
	{
		//long long cur=std::chrono::system_clock::now();
		//auto &b=mp.try_emplace(clientId, defLimit, defWindow).first->second;

		auto b=getBucket(clientId);
		lock_guard<mutex> lg(b->m);
		while(!b.q.empty()&&ts-b.q.front()>=b.window) b.q.pop_front();

		if(b.q.size()>=b.limit) 
		{
			long long retry=b.window-(ts-b.q.front());
			return call429(retry);
		}

		b.q.emplace_back{ts};
		return call200();
	}

	void reconfigure(const string& clientId, int limit, int window)
	{
		auto b=getBucket(clientId);
		lock_guard<mutex> lg(b->m);
		b->limit=limit;
		b->window=window;
		//auto&b=mp.try_emplace(clientId, limit, window).first->second;
		//b.limit=limit;
		//b.window=window;
	}

	void reconfigure(int limit, int window)
	{
		lock_gaurd<mutex> lg(mp_m);
		defLimit=limit;
		defWindow=window;
	}
}
