#ifndef HEADER_GUARD_VA_QUEUE_INCLUDED
#define HEADER_GUARD_VA_QUEUE_INCLUDED "Queue.hpp"

#include <type_traits>
#include <initializer_list>

#include "VaException.hpp"

// Implemented via circular buffer
namespace VaQueue
{
	namespace _queue_detail
	{
		template <typename Data_t, size_t capasity_>
		class QueueCoreInterface
		{
		protected:
			// Functions on elements:
				virtual void insert(size_t index, const Data_t& ) = 0;
				virtual void insert(size_t index,       Data_t&&) = 0;

				virtual Data_t& get(size_t index) = 0;

				virtual Data_t&& remove(size_t index) = 0
		};

		template <typename Data_t, size_t capasity_>
		class PolymorphicCore : protected QueueCoreInterface<Data_t, capasity_>
		{
		private:
			// Variables:
				Data_t* buf_[capasity_];

		protected:
			// Dtor:
				~PolymorphicCore();

			// Ctor:
				PolymorphicCore();

			// Copy stuff:
				PolymorphicCore           (const PolymorphicCore&);
				PolymorphicCore& operator=(const PolymorphicCore&);

			// Move stuff:
				PolymorphicCore           (PolymorphicCore&&) = default;
				PolymorphicCore& operator=(PolymorphicCore&&) = default;

			// Functions on elements:
				virtual void insert(size_t index, const Data_t& ) override;
				virtual void insert(size_t index,       Data_t&&) override;

				virtual Data_t& get(size_t index) override;

				virtual Data_t&& remove(size_t index) override;
		};

		template <typename Data_t, size_t capasity_>
		class NormalCore : protected QueueCoreInterface<Data_t, capasity_>
		{
		private:
			// Variables:
				Data_t buf_[capasity_];

		protected:
			// Dtor:
				~NormalCore() = default;

			// Ctor:
				NormalCore() = default;

			// Copy stuff:
				NormalCore           (const NormalCore&) = default;
				NormalCore& operator=(const NormalCore&) = default;

			// Move stuff:
				NormalCore           (NormalCore&&) = default;
				NormalCore& operator=(NormalCore&&) = default;

			// Functions on elements:
				virtual void insert(size_t index, const Data_t& ) override;
				virtual void insert(size_t index,       Data_t&&) override;

				virtual Data_t& get(size_t index) override;

				virtual Data_t&& remove(size_t index) override;
		};

		//-----------------------------------------------------------
		// Implementation:
		//-----------------------------------------------------------

		using namespace VaExc;

		// PolymorphicCore impl:
			// Dtor:
				template <typename Data_t, size_t capasity_>
				PolymorphicCore<Data_t, capasity_>::~PolymorphicCore()
				{
					for (size_t i = 0; i < capasity_; ++i)
					{
						if (buf_[i] != nullptr)
						{
							delete buf_[i];
							buf_[i] = nullptr;
						}
					}
				}

			// Ctor:
				template <typename Data_t, size_t capasity_>
				PolymorphicCore<Data_t, capasity_>::PolymorphicCore() :
					buf_ ()
				{
					for (size_t i = 0; i < capasity_; ++i)
					{
						buf_[i] = nullptr;
					}
				}

			// Copy stuff:
				template <typename Data_t, size_t capasity_>
				PolymorphicCore<Data_t, capasity_>::PolymorphicCore(const PolymorphicCore<Data_t, capasity_>& that)
				{
					for (size_t i = 0; i < capasity_; ++i)
					{
						if (that.buf_[i] == nullptr) buf_[i] = nullptr;
						else buf_[i] = new Data_t(*that.buf_[i]);
					}
				}

				template <typename Data_t, size_t capasity_>
				PolymorphicCore<Data_t, capasity_>&
				PolymorphicCore<Data_t, capasity_>::operator=(const PolymorphicCore<Data_t, capasity_>& that)
				{
					this->~PolymorphicCore();
					this->PolymorphicCore(that);
				}

			// Functions on elements:
				template <typename Data_t, size_t capasity_>
				void PolymorphicCore<Data_t, capasity_>::insert(size_t index, const Data_t& data)
				{
					if (index >= capasity_)
					{
						throw Exception(ArgMsg("Index out of bounds: %zu", index), VAEXC_POS);
					}

					if (buf_[index] != nullptr)
					{
						buf_[index] = new Data_t(data);
					}
					else
					{
						*buf_[index] = data;
					}
				}

				template <typename Data_t, size_t capasity_>
				void PolymorphicCore<Data_t, capasity_>::insert(size_t index, Data_t&& data)
				{
					if (index >= capasity_)
					{
						throw Exception(ArgMsg("Index out of bounds: %zu", index), VAEXC_POS);
					}

					if (buf_[index] != nullptr)
					{
						buf_[index] = new Data_t(std::move(data));
					}
					else
					{
						*buf_[index] = std::move(data);
					}
				}

				template <typename Data_t, size_t capasity_>
				Data_t& PolymorphicCore<Data_t, capasity_>::get(size_t index)
				{
					if (index >= capasity_)
					{
						throw Exception(ArgMsg("Index out of bounds: %zu", index), VAEXC_POS);
					}

					if (buf_[index] == nullptr)
					{
						throw Exception(ArgMsg("No element by index: %zu", index), VAEXC_POS);
					}

					return buf_[index];
				}

				template <typename Data_t, size_t capasity_>
				Data_t&& PolymorphicCore<Data_t, capasity_>::remove(size_t index)
				{
					if (index >= capasity_)
					{
						throw Exception(ArgMsg("Index out of bounds: %zu", index), VAEXC_POS);
					}

					if (buf_[index] == nullptr)
					{
						throw Exception(ArgMsg("No element by index: %zu", index), VAEXC_POS);
					}

					Data_t* toReturn = buf_[index];

					buf_[index] = nullptr;

					return std::move(*toReturn);
				}

		// NormalCore impl:
			// Functions on elements:
				template <typename Data_t, size_t capasity_>
				void NormalCore<Data_t, capasity_>::insert(size_t index, const Data_t& data)
				{
					if (index >= capasity_)
					{
						throw Exception(ArgMsg("Index out of bounds: %zu", index), VAEXC_POS);
					}

					buf_[index] = data;
				}

				template <typename Data_t, size_t capasity_>
				void NormalCore<Data_t, capasity_>::insert(size_t index, Data_t&& data)
				{
					if (index >= capasity_)
					{
						throw Exception(ArgMsg("Index out of bounds: %zu", index), VAEXC_POS);
					}

					buf_[index] = data;
				}

				template <typename Data_t, size_t capasity_>
				Data_t& NormalCore<Data_t, capasity_>::get(size_t index)
				{
					if (index >= capasity_)
					{
						throw Exception(ArgMsg("Index out of bounds: %zu", index), VAEXC_POS);
					}

					return buf_[index];
				}

				template <typename Data_t, size_t capasity_>
				Data_t&& NormalCore<Data_t, capasity_>::remove(size_t index)
				{
					if (index >= capasity_)
					{
						throw Exception(ArgMsg("Index out of bounds: %zu", index), VAEXC_POS);
					}

					return std::move(buf_[index]);
				}

	} // namespace _queue_detail_impl

	//-----------------------------------------------------------
	// Queue itself:
	//-----------------------------------------------------------


	namespace _queue
	{
		using namespace VaExc;

		template <typename Data_t, size_t capasity_>
		class Queue :
			protected std::conditional_t
			<
				std::is_polymorphic<Data_t>::value,
				_queue_detail::PolymorphicCore<Data_t, capasity_>,
				_queue_detail::NormalCore<Data_t, capasity_>
			>
		{
		protected:
			// Variables:
				size_t beg_;
				size_t end_;

			// Typedef:
				using Ancestor = std::conditional_t
				<
					std::is_polymorphic<Data_t>::value,
					_queue_detail::PolymorphicCore<Data_t, capasity_>,
					_queue_detail::NormalCore<Data_t, capasity_>
				>;

		public:
			// Dtor:
				~Queue() = default;

			// Ctors:
				Queue();
				explicit Queue(std::initializer_list<Data_t> list);

			// Copy stuff:
				Queue           (const Queue&) = default;
				Queue& operator=(const Queue&) = default;

			// Move stuff:
				Queue           (Queue&&) = default;
				Queue& operator=(Queue&&) = default;


			// Functions on elements:
				Queue& push_back(const Data_t& );
				Queue& push_back(      Data_t&&);

				Data_t&& pop_front();

				Data_t& at(size_t index);

				inline size_t capasity() const { return capasity_; }

				inline size_t size() const { return end_ - beg_; }

			// Assertion:
				inline void throwIfNotOk() const;
		};

		//-----------------------------------------------------------
		// Implementation:
		//-----------------------------------------------------------

			// Ctors:
				template <typename Data_t, size_t capasity_>
				Queue<Data_t, capasity_>::Queue() :
					Ancestor(),
					beg_ (0),
					end_ (0)
				{}

				template <typename Data_t, size_t capasity_>
				Queue<Data_t, capasity_>::Queue(std::initializer_list<Data_t> list) :
					Ancestor(),
					beg_ (0),
					end_ (0)
				{
					if (list.size() > capasity_)
					{
						throw Exception("Initializer list has too many elements"_msg, VAEXC_POS);
					}

					for (auto i = list.begin(); end_ != capasity_ && i != list.end(); ++i, ++end_)
					{
						Ancestor::insert(end_, std::move(*i));
					}

					throwIfNotOk();
				}

			// Functions on elements:
				template <typename Data_t, size_t capasity_>
				Queue<Data_t, capasity_>& Queue<Data_t, capasity_>::push_back(const Data_t& data)
				{
					throwIfNotOk();

					if (end_ - beg_ == capasity_)
					{
						throw Exception("Queue overflow"_msg, VAEXC_POS);
					}

					Ancestor::insert(end_ % capasity_, data);

					++end_;

					return *this;
				}

				template <typename Data_t, size_t capasity_>
				Queue<Data_t, capasity_>& Queue<Data_t, capasity_>::push_back(Data_t&& data)
				{
					throwIfNotOk();

					if (end_ - beg_ == capasity_)
					{
						throw Exception("Queue overflow"_msg, VAEXC_POS);
					}

					Ancestor::insert(end_ % capasity_, std::move(data));

					++end_;

					return *this;
				}

				template <typename Data_t, size_t capasity_>
				Data_t&& Queue<Data_t, capasity_>::pop_front()
				{
					throwIfNotOk();

					if (end_ == beg_)
					{
						throw Exception("Can't pop from empty Queue"_msg, VAEXC_POS);
					}

					size_t toDelete = beg_;

					// Fixing beg_ and end_:
					size_t newSize = end_ - beg_ - 1;
					beg_ = (beg_ + 1) % capasity_;
					end_ = beg_ + newSize;

					return Ancestor::remove(toDelete);
				}

				template <typename Data_t, size_t capasity_>
				Data_t& Queue<Data_t, capasity_>::at(size_t index)
				{
					throwIfNotOk();

					if (index >= end_ - beg_)
					{
						throw Exception("Access out of bounds"_msg, VAEXC_POS);
					}

					return Ancestor::get((beg_ + index) % capasity_);
				}

			// Assertion:
				template <typename Data_t, size_t capasity_>
				inline void Queue<Data_t, capasity_>::throwIfNotOk() const
				{
					if (beg_ >= capasity_)
					{
						throw Exception("Queue: beg_ variable is not OK"_msg, VAEXC_POS);
					}

					if (end_ < beg_ || end_ > beg_ + capasity_)
					{
						throw Exception("Queue: end_ variable is not OK"_msg, VAEXC_POS);
					}
				}

	} // namespace _queue

	template <typename Data_t, size_t capasity_>
	using Queue = _queue::Queue<Data_t, capasity_>;

}

#endif /* HEADER_GUARD_VA_QUEUE_INCLUDED */