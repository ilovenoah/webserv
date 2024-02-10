#ifndef RESULT_HPP
#define RESULT_HPP

template <typename T, typename E>
class Result {
	private:
		Result();

		enum tag {
				OK,
				ERROR,
		};
		tag t;
		T ok;
		E error;

	public:
		Result(T const &_ok) : t(OK), ok(_ok) {}
		Result(E const &_error) : t(ERROR), error(_error) {}
		Result(Result const &sourceResult) { *this = sourceResult; }
		Result &operator=(Result const &sourceResult) {
				if (this != &sourceResult) {
						t = sourceResult.t;
						switch (t) {
								case OK:
										ok = sourceResult.ok;
										break;
								case ERROR:
										error = sourceResult.error;
										break;
						}
				}
				return *this;
		}

		bool isOK() const { return t == OK; }
		bool isError() const { return t == ERROR; }
		T const &getOk() const { return ok; }
		E const &getError() const { return error; }
};

#endif
