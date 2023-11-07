#pragma once

#include <QObject>

using task_t = std::tuple<int, std::string, std::string, std::string >;
using task_list_t = std::vector<task_t>;

namespace types_impl {
	
	//TABLE
	template <class Tuple>
	std::string tuple_info_name() {
		if constexpr (std::is_same_v<Tuple, task_t>)
			return "tasks";
		return "";
	}

	//TABLE, CUSTOM SELECT
	template <class Tuple>
	std::pair<std::string, std::string> tuple_info() {
		if constexpr (std::is_same_v<Tuple, task_t>)
			return { tuple_info_name <Tuple>(),
				"id, " 
				"COALESCE(title, ''::text) AS title, "
				"COALESCE(to_char(updated_at, 'DD Month YYYY HH24:MI'::text), ''::text) AS format_updated_at, "
				"COALESCE(\"left\"(full_text, 30), ''::text) AS formated_full_text"
		};
		return { "","*" }; //SELECT *
	}

	template <class Tuple>
	std::string field_info(int field) {
		if constexpr (std::is_same_v<Tuple, task_t>) {
			static const auto fields = std::vector{ "id", "title", "created_at", "full_text" };
			return fields[field];
		};
		return "";
	}

}
