#ifndef cache_hpp
#define cache_hpp

	template<class X, class Y>
	class Cache
	{
	public:
	
		//CONSTRUCTOR THAT TAKE A FUNCTION
		Cache(const std::function<Y(X)> _f){
			f = _f;	
		}
	
		//REDEFINITION OF THE () OPERATOR TO HAVE A FUNCTOR
		Y operator()(X key) const {
			auto index = value.find(key);
			if(index != value.end())
				return value[key];
			else
				return calculateData(key);
		}
	
		//TO CLEAR ALL MAP VALUE
		void clear(){
			value.clear;
		}
	
		//TO DELETE SPECIFIC KEY VALUE
		void erase(X key){
			value.erase(key);
		}

	private:

		std::function<Y(X)> f; 
		mutable std::map <X, Y> value;

		//FUNCTION TO CALCULATE VALUE
		Y calculateData(X key) const {
			put(key, f(key));
			return value[key];
		}
		//SET NEW DATA
		template<class Z>
		void put(X key, Z data) const {
			value[key] = static_cast<Y>(data);
		}
	
	};


#endif
