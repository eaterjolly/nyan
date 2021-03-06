// Copyright 2016-2016 the nyan authors, LGPLv3+. See copying.md for legal info.
#ifndef NYAN_NYAN_CONTAINER_H_
#define NYAN_NYAN_CONTAINER_H_

#include <iterator>

#include "nyan_value.h"


namespace nyan {

class NyanValueContainer;


/**
 * Base class for container iterators.
 * Inherited from and implemented for each nyan container
 * in order to support iteration.
 *
 * The child class adds stuff so it can iterate over whatever the
 * target container needs for iteration state storage.
 *
 * The begin() and end() functions of the container class
 * instanciate this by wrapping it in the ContainerIterator below.
 */
template<typename iter_type>
class ContainerIterBase : public std::iterator<std::forward_iterator_tag, iter_type> {
public:
	using this_type = ContainerIterBase<iter_type>;
	using elem_type = iter_type;

	ContainerIterBase() = default;
	virtual ~ContainerIterBase() = default;

	/**
	 * Advance the iterator to the next element.
	 */
	virtual this_type &operator ++() = 0;

	/**
	 * Get the element the iterator is currently pointing to.
	 */
	virtual iter_type &operator *() const = 0;

	/**
	 * Compare if both iterators are pointing
	 * to the same container position.
	 */
	bool operator ==(const ContainerIterBase &other) const {
		return (typeid(*this) == typeid(other)) and this->equals(other);
	}

protected:
	/**
	 * Actually perform the comparison if both iterators
	 * point to the same element.
	 */
	virtual bool equals(const ContainerIterBase &other) const = 0;
};


/**
 * Nyan container iterator wrapper class.
 * Wraps the ContainerIterBase so we can have virtual calls.
 */
template<typename T>
class ContainerIterator : public std::iterator<std::forward_iterator_tag, T> {
public:
	using elem_type = T;
	using real_iterator = ContainerIterBase<elem_type>;


	ContainerIterator() = default;
	ContainerIterator(std::unique_ptr<real_iterator> &&real) noexcept
		:
		iter{std::move(real)} {}

	ContainerIterator(const ContainerIterator &other)
		:
		iter{std::make_unique(other)} {}

	ContainerIterator(ContainerIterator &&other) noexcept
		:
		iter{std::move(other.iter)} {}

	ContainerIterator &operator =(const ContainerIterator &other) {
		this->iter = std::make_unique(other);
	}

	ContainerIterator &operator =(ContainerIterator &&other) noexcept {
		this->iter = std::move(other);
	}

	virtual ~ContainerIterator() = default;

	/**
	 * Advance the inner iterator to the next element.
	 */
	ContainerIterator &operator ++() {
		++(*this->iter);
		return *this;
	}

	/**
	 * Get the element the inner iterator points to.
	 */
	elem_type &operator *() const {
		return *(*this->iter);
	}

	/**
	 * Check if this iterator points to the same container element
	 * as the other iterator.
	 */
	bool operator ==(const ContainerIterator& other) const {
		return (this->iter == other.iter) or (*this->iter == *other.iter);
	}

	/**
	 * Check if the iterator does not point to the same container element
	 * as the other iterator.
	 */
	bool operator !=(const ContainerIterator& other) const {
		return not (*this == other);
	}

protected:
	/**
	 * The real iterator.
	 * Just wrapped here to enable virtual function calls.
	 */
	std::unique_ptr<real_iterator> iter;
};


/**
 * NyanValue that can store other NyanValues.
 */
class NyanContainer : public NyanValue {
public:
	using iterator = ContainerIterator<NyanValue>;
	using const_iterator = ContainerIterator<const NyanValue>;

	NyanContainer();
	virtual ~NyanContainer() = default;

	/**
	 * Return the number of elements in this container.
	 */
	virtual size_t size() const = 0;

	/**
	 * Add the given value to this container.
	 * @returns if the value was added successfully,
	 * false if it was already in there.
	 */
	virtual bool add(NyanValueContainer &&value) = 0;

	/**
	 * Test if this value is in the container.
	 */
	virtual bool contains(NyanValue *value) = 0;

	/**
	 * Remove the given value from the container if it is in there.
	 * @returns if if was removed successfully.
	 */
	virtual bool remove(NyanValue *value) = 0;

	/**
	 * Get an iterator to the first element in that container.
	 */
	virtual iterator begin() = 0;

	/**
	 * Get an iterator to the slot beyond the last element in the container.
	 */
	virtual iterator end() = 0;

	/**
	 * Get a constant iterator to the first element in that container.
	 */
	virtual const_iterator begin() const = 0;

	/**
	 * Get a constant iterator to the slot beyond the last element in the
	 * container.
	 */
	virtual const_iterator end() const = 0;

	/**
	 * Guarantee a const_iterator beginning.
	 */
	const_iterator cbegin() const { return this->begin(); };

	/**
	 * Guarantee a const_iterator end.
	 */
	const_iterator cend() const { return this->end(); };
};

} // namespace nyan

#endif
