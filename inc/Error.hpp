#ifndef ERROR_HPP
#define ERROR_HPP

#include "Result.hpp"

template <typename E>
class Error {
	private:
		Error() {}

		E e;

	public:
		Error(E const &_e) : e(_e) {}
		Error(Error const &sourceError) { *this = sourceError; }
		Error &operator=(Error const &sourceError) {
				if (this != &sourceError) {
						e = sourceError.e;
				}
				return *this;
		}
		~Error() {}
		template <typename T, typename V>
		operator Result<T, V>() const {
				return Result<T, V>(e);
		}
};
#endif
