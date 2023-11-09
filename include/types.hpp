#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <format>

namespace null_values {

	template <class T>
	T get() {
		if constexpr (std::is_same_v<T, int>)
			return std::numeric_limits<int>::min();
		if constexpr (std::is_same_v<T,std::string>)
			return "_null";
	}

	bool is_null(const auto & x) {
		return get<std::remove_cvref_t<decltype(x)>>() == x;
	}

	template<class... T>
	void _fill_tuple_null(std::tuple<T...> & tuple) {
		std::apply([&](auto&... elem) {
			((elem = get<T>()), ...);
		}, tuple);
	}

}
template <class D, class Tuple> requires std::_Is_specialization_v<Tuple, std::tuple>
struct BasicTypeDB{

	BasicTypeDB() {
		null_values::_fill_tuple_null(tp);
	}

	static std::string tuple_info_name() { return D::tuple_info_name_override(); }
	static std::string tuple_info_custom_select() { return  D::tuple_info_custom_select_override(); }
	static std::string field_info(int field) { return  D::field_info_override(field); }

	using tuple_t = Tuple;
	static constexpr int tuple_size = std::tuple_size_v<Tuple>;
	Tuple tp;
};

struct Task_Tuple : public BasicTypeDB<Task_Tuple, 
					std::tuple<int, std::string, std::string, std::string, int> > {
	static std::string tuple_info_name_override() { return "tasks"; }
	static std::string tuple_info_custom_select_override() { 
			return std::format("id, "
			"COALESCE(title, '{}'::text) AS title, "
			"COALESCE(to_char(updated_at, 'DD Month YYYY HH24:MI'::text), '{}'::text) AS format_updated_at, "
			"COALESCE(\"left\"(full_text, 30), '{}'::text) AS formated_full_text,"
			"COALESCE(color_schemas_id, {})", 
			null_values::get<std::string>(),
			null_values::get<std::string>(),
			null_values::get<std::string>(),
			null_values::get<int>()
			);
	}
	static std::string field_info_override(int field) {
		static const auto fields = std::vector{ "id", "title", "created_at", "full_text", "color_schemas_id"};
		return fields[field];
	}
};

struct Color_Tuple : public BasicTypeDB <Color_Tuple, std::tuple<int, std::string>> {
	static std::string tuple_info_name_override() { return "color_schemas"; }
	static std::string tuple_info_custom_select_override() {
		return "*";
	}
	static std::string field_info_override(int field) {
		static const auto fields = std::vector{ "id", "main_color_code" };
		return fields[field];
	}
};


using Task_Tuple_List = std::vector<Task_Tuple>;