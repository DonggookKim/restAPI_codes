// restAPI_ex.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <iostream>
#include <string>
#include <windows.data.json.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

void display_json(
	json::value const & jvalue,
	utility::string_t const & prefix)
{
	std::string prefixS = utility::conversions::to_utf8string(prefix);

	std::string jvalueS = utility::conversions::to_utf8string(jvalue.serialize());

	std::cout << prefixS << jvalueS << std::endl << std::endl;

	json::value obj = json::value::parse(jvalue.serialize());

	std::cout << utility::conversions::to_utf8string(obj.at(U("birth_year")).as_string()) << "\n\n";

}

json::value parsing_json(json::value const& jvalue) {

	return json::value::parse(jvalue.serialize());
	
}

// return keyword string
std::string take_json_string(json::value const& jvalue, const wchar_t* keyword) {
	json::value parsed = parsing_json(jvalue);
	return utility::conversions::to_utf8string(parsed.at(keyword).as_string());
}
// return keyword int
int take_json_int(json::value const& jvalue, const wchar_t* keyword) {
	json::value parsed = parsing_json(jvalue);
	return parsed.at(keyword).as_number().to_int32();
}
// return keyword array
std::string take_json_array(json::value const& jvalue, const wchar_t* keyword) {
	json::value parsed = parsing_json(jvalue);
	json::value arrayv = parsed.at(keyword);
	json::array arrayJ = arrayv.as_array();

	std::cout << "arrayJ 0 : " << utility::conversions::to_utf8string(arrayJ[0].as_string()) << "\n";
	std::cout << "arrayJ 1 : " << utility::conversions::to_utf8string(arrayJ[1].as_string()) << "\n";


	return "";
}


pplx::task<http_response> make_task_request(
	http_client & client,
	method mtd,
	json::value const & jvalue, const wchar_t* add_path)
{
	return (mtd == methods::GET || mtd == methods::HEAD) ?
		client.request(mtd, add_path) :
		client.request(mtd, add_path, jvalue);
}

void make_request(
	http_client & client,
	method mtd,
	json::value const & jvalue, const wchar_t* add_path, json::value& ret)
{
	std::cout << "REQUEST START\n";
	auto qq = make_task_request(client, mtd, jvalue, add_path)
		.then([&](http_response response)
	{
		if (response.status_code() == status_codes::OK)
		{
			// success case. return extracted json.
			return response.extract_json();
		}
		else {
			// fail case
		}
		return pplx::task_from_result(json::value());
	})
		.then([&](pplx::task<json::value> previousTask)
	{
		try
		{
			// success case.
			json::value prevJson = previousTask.get();
			//display_json(prevJson, L"R: ");
			ret = prevJson;
			return prevJson;
		}
		catch (http_exception const & e)
		{
			// fail case
			std::cout << e.what() << std::endl;
			return json::value::null();
		}
	});
	qq.wait();
	std::cout << "REQUEST WAIT END\n";
	display_json(ret, L"AFTER END : ");
}

int main()
{
	http_client client(U("https://swapi.co/api/"));
	auto getvalue = json::value::array();
	json::value resp;

	make_request(client, methods::GET, getvalue, L"/people/1",resp);
	std::cout << "\n\nTAKE JSON NAME : " << take_json_string(resp, U("birth_year"));
	std::cout << "\n\nTAKE JSON NAME : " << take_json_array(resp, U("vehicles"));
/*
	auto putvalue = json::value::object();
	putvalue[L"one"] = json::value::string(L"100");
	putvalue[L"two"] = json::value::string(L"200");

	std::cout << "\nPUT (add values)\n";
	display_json(putvalue, L"S: ");
	make_request(client, methods::PUT, putvalue, L"/people/1");

	auto getvalue = json::value::array();

	std::cout << "\nGET (get some values)\n";
	display_json(getvalue, L"S: ");
	make_request(client, methods::GET, getvalue, L"/people/1");

	auto postvalue = json::value::array();
	postvalue[0] = json::value::string(L"one");

	std::cout << "\nPOST (get some values)\n";
	display_json(getvalue, L"S: ");
	make_request(client, methods::POST, getvalue, L"/planets/1");

*/
	return 0;
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
