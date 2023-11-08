#pragma once

#include <QObject>

template <class D, class Tuple> requires std::_Is_specialization_v<Tuple, std::tuple>
struct BasicTypeDB{
	static std::string tuple_info_name() { return D::tuple_info_name_override(); }
	static std::string tuple_info_custom_select() { return  D::tuple_info_custom_select_override(); }
	static std::string field_info(int field) { return  D::field_info_override(field); }

	using tuple_t = Tuple;
	static constexpr int tuple_size = std::tuple_size_v<Tuple>;
	Tuple tp;
};

struct Task_Tuple : public BasicTypeDB<Task_Tuple, std::tuple<int, std::string, std::string, std::string> > {
	static std::string tuple_info_name_override() { return "tasks"; }
	static std::string tuple_info_custom_select_override() { return "id, "
			"COALESCE(title, ''::text) AS title, "
			"COALESCE(to_char(updated_at, 'DD Month YYYY HH24:MI'::text), ''::text) AS format_updated_at, "
			"COALESCE(\"left\"(full_text, 30), ''::text) AS formated_full_text";
	}
	static std::string field_info_override(int field) {
		static const auto fields = std::vector{ "id", "title", "created_at", "full_text" };
		return fields[field];
	}
};

using Task_Tuple_List = std::vector<Task_Tuple>;