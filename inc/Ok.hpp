#ifndef OK_HPP
#define OK_HPP

#include "Result.hpp"

template <typename T>
class Ok {
	private:
		Ok() {}

		T t;

	public:
		Ok(T const &_t) : t(_t) {}
		Ok(Ok const &sourceOk) { *this = sourceOk; }
		Ok &operator=(Ok const &sourceOk) {
				if (this != &sourceOk) {
						t = sourceOk.t;
				}
				return *this;
		}
		~Ok() {}
		template <typename V, typename E>
		operator Result<V, E>() const {
				return Result<V, E>(t);
		}
};

#endif

