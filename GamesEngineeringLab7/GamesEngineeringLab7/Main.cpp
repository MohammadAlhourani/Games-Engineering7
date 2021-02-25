#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <stack>
#include <queue>

using namespace std;

const int n = 5;

int m_start = 0;
int m_end = 0;
int m_empty = n;
int m_ticket = 0;
int m_nextTicket = 0;

int buffer[n];

mutex* m_producerLock = new mutex;
mutex* m_consumerLock = new mutex;


void producer()
{
	auto threadID = this_thread::get_id();

	while (true)
	{
		while (m_empty == 0)
		{
			cout << "Producer Spinning " << threadID << endl;
			this_thread::sleep_for(chrono::seconds(2));
		}

		m_producerLock->lock();
		if (m_empty != 0)
		{
			m_empty -= 1;
			m_producerLock->unlock();

			m_producerLock->lock();
			buffer[m_end] = m_ticket++;
			cout << buffer[m_end] << " Thread: " << threadID << endl;
			m_end = (m_end + 1) % n;
			m_producerLock->unlock();
		}
		else
		{
			m_producerLock->unlock();
		}
	}

	cout << "Production Finished" << endl;
}

void consumer()
{
	auto threadID = this_thread::get_id();

	while (true)
	{
		int ticket = m_nextTicket++;

		while (ticket != buffer[m_start])
		{
			continue;
		}

		this_thread::sleep_for(chrono::seconds(1));
		m_consumerLock->lock();
		cout << buffer[m_start] << " Consumer  Thread: " << threadID << endl;
		m_start = (m_start + 1) % n;
		m_consumerLock->unlock();

		m_empty += 1;
	}
}

int main(void)
{
	std::vector<std::thread*> producerThreads;
	std::vector<std::thread*> consumerThreads;

	for (int i = 0; i < n; i++)
	{
		buffer[i] = -1;
	}

	for (int i = 0; i < 3; i++)
	{
		std::thread producer(producer);
		producer.detach();
		producerThreads.push_back(&producer);

		std::thread consumer(consumer);
		consumer.detach();
		consumerThreads.push_back(&consumer);
	}

	cin.get();
	return 0;
}