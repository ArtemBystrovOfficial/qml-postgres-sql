#pragma once 

#include <iostream>
#include <stdexcept>
#include <pqxx/pqxx>
#include <tuple>
#include <types.hpp>
#include <bitset>

class DataBaseAccess {
public:
	static DataBaseAccess& Instanse(const std::string& connection_query = "") {
		static DataBaseAccess base(connection_query);
		return base;
	}

	struct FilterSelectPack {
		std::optional<bool> sort_asc;
		std::string sort_field;

		std::optional<std::string> search_text;
		std::vector<std::string> search_fields;
		//::optional<std::string> filtered;
	};

	template<class Tuple>
	std::optional<std::vector<Tuple>> Select(const FilterSelectPack& pack);

	template <typename Tuple, std::size_t TupSize = std::tuple_size_v<Tuple>>
	bool Insert(const Tuple&);

	template <typename Tuple, std::size_t TupSize = std::tuple_size_v<Tuple>>
	bool Update(const Tuple&, const std::bitset<TupSize>&);

	template<class Tuple>
	bool Delete(const Tuple& tuple);

	template<class T>
	std::optional <T> specialSelect11(const std::string&);
private:
	DataBaseAccess(const std::string& connection_query);
//INSERT
	template <typename Tuple, std::size_t... Is>
	std::string insertImpl(const Tuple& tp, std::index_sequence<Is...>);
//UPDATE
	template <typename Tuple, std::size_t... Is>
	std::string updateImpl(const Tuple& tp,const std::bitset<sizeof...(Is)>&, std::index_sequence<Is...>);
//SELECT + FILTERS
	std::string searchRule(const FilterSelectPack& pack);
	std::string sortRule(const FilterSelectPack& pack);

//OTHER
	std::string convertType(const auto & x) {
		using type = std::remove_cvref_t<decltype(x)>;
		if constexpr (std::is_same_v<type, std::string>)
			return x.empty() ? "NULL" : std::format("'{}'", x);
		if constexpr (requires { x + 1; })
			return std::to_string(x);
		return "";
	}

	pqxx::connection m_conn;
};

//SELECT

template<class Tuple>
inline std::optional<std::vector<Tuple>> DataBaseAccess::Select(const FilterSelectPack& pack) {
	try {
		auto [from, sel] = types_impl::tuple_info<Tuple>();
		auto query = std::format(
			"SELECT {} FROM {} {} {}",
			sel, from, searchRule(pack), sortRule(pack)
		);
		pqxx::work w(m_conn);
		pqxx::result res = w.exec(query);

		std::vector <Tuple> m_out;
		for (auto row : res) {
			m_out.push_back(Tuple{});
			row.to(m_out.back());
		}

		return m_out;
	}
	catch (...) {
		return std::nullopt;
	}
}

//INSERT

template<typename Tuple, std::size_t TupSize>
inline bool DataBaseAccess::Insert(const Tuple& tp) {
	std::string query = insertImpl(tp, std::make_index_sequence<TupSize>{});
	try {
		pqxx::work w(m_conn);
		w.exec(query);
		w.commit(); //TODO UNDO LIST
	}
	catch (...) {
		return false;
	}
	return true;
}

template<typename Tuple, std::size_t ...Is>
inline std::string DataBaseAccess::insertImpl(const Tuple& tp, std::index_sequence<Is...>) {
		size_t index = 0;
		std::string out = "INSERT INTO ";
		out += types_impl::tuple_info_name<Tuple>();
		out += " VALUES (DEFAULT, ";
		auto printElem = [&index, &out, this](const auto& x) {
			if (index != 0) { //id skip
				out += convertType(x);
				out += ", ";
			}
			index++;
		};
		(printElem(std::get<Is>(tp)), ...);
		out.erase(out.end() - 2, out.end());
		out += ") ";
		return out;
}

//UPDATE 

template<typename Tuple, std::size_t TupSize>
inline bool DataBaseAccess::Update(const Tuple& tp, const std::bitset<TupSize>& update_set) {
	std::string query = std::format(
		"UPDATE {} SET {} WHERE {} = {}",
		types_impl::tuple_info_name<Tuple>(),
		updateImpl(tp, update_set, std::make_index_sequence<TupSize>{}),
		types_impl::field_info<Tuple>(0),
		std::get<0>(tp)
	);
	try {
		pqxx::work w(m_conn);
		w.exec(query);
		w.commit(); //TODO UNDO LIST
	}
	catch (...) {
		return false;
	}
	return true;
}

template<typename Tuple, std::size_t ...Is>
inline std::string DataBaseAccess::updateImpl(const Tuple& tp, const std::bitset<sizeof...(Is)>& update_set, std::index_sequence<Is...>) {
	size_t index = 0;
	std::string out;
	auto printElem = [&index, &out, &update_set, this](const auto& x) {
		if (update_set[index]) {
			out += types_impl::field_info<Tuple>(index);
			out += " = ";
			out += convertType(x);
			out += ", ";
		}
		index++;
	};
	(printElem(std::get<Is>(tp)), ...);
	out.erase(out.end() - 2, out.end());
	return out;
}

//DELETE
template<class Tuple>
inline bool DataBaseAccess::Delete(const Tuple& tp) {
	try {
		pqxx::work w(m_conn);
		w.exec(std::format("DELETE FROM {} WHERE id = {}",
				types_impl::tuple_info_name<Tuple>(),
				std::get<0>(tp)
			   )
		);
		w.commit();
	}
	catch (...) {
		return false;
	}
	return true;
}

template<class T>
inline std::optional <T> DataBaseAccess::specialSelect11(const std::string& query) {
	try {
		pqxx::work w(m_conn);
		pqxx::row res = w.exec1(query);
		return std::move(res[0].as<T>());
	}
	catch (...) {
		return std::nullopt;
	}
}