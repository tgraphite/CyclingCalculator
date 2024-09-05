#include "io.h"

#include <vector>
#include <string>
#include <set>
#include <fstream>
#include <iostream>

#include <curl/curl.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

/**
 * @brief 按分隔符拆分字符串
 *
 * 将给定的字符串按照指定的分隔符进行拆分，并返回拆分后的子字符串向量。
 *
 * @param str 要拆分的字符串
 * @param delim 分隔符
 *
 * @return 拆分后的子字符串向量
 */
std::vector<std::string> splitByDelimiter(const std::string& str, const std::string& delim)
{
	std::vector<std::string> tokens;
	size_t prev = 0, pos = 0;
	while (pos < str.length() && prev < str.length())
	{
		pos = str.find(delim, prev);
		if (pos == std::string::npos)
			pos = str.length();
		std::string token = str.substr(prev, pos - prev);
		if (!token.empty())
			tokens.push_back(token);
		else
			tokens.push_back("");
		prev = pos + delim.length();
	}
	return tokens;
}

/**
 * @brief 写入回调函数
 *
 * 将给定的内容写入到用户指定的字符串中，并返回写入的字节数。
 *
 * @param contents 内容指针
 * @param size 单个元素的大小
 * @param nmemb 元素的数量
 * @param userp 用户指针
 *
 * @return 写入的字节数
 */
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

/**
 * @brief 解析 Fit 文件并发送至服务器
 *
 * 将指定路径下的 Fit 文件读取为二进制数据，并通过 HTTP POST 请求发送至服务器进行解析。
 * 解析结果将写入到 "response.json" 文件中。
 *
 * @param path Fit 文件路径
 *
 * @return 成功返回 0，失败返回 1
 */
int InterpidsportFitService(const std::string& path, const std::string& output_path)
{
	// 打开文件并读取内容
	std::ifstream file(path, std::ios::binary);
	if (!file) {
		std::cerr << "Error opening file " << path << std::endl;
		return 1;
	}

	std::vector<char> fileData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	// 初始化curl
	CURL* curl = curl_easy_init();
	if (!curl) {
		std::cerr << "Error initializing curl" << std::endl;
		return 1;
	}

	CURLcode res;
	std::string readBuffer;

	curl_easy_setopt(curl, CURLOPT_URL, "http://data.intrepidsport.cn:8803/fit/decode/v1");
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, fileData.data());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, fileData.size());

	// 设置请求头
	struct curl_slist* headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/octet-stream");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	// 设置回调函数以接收响应
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

	// 发送请求
	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
	}
	else {
		std::ofstream outFile(output_path);
		if (outFile) {
			outFile << readBuffer;
			outFile.close();
			std::cout << "Convert " << path << " to json" << std::endl;
		}
		else {
			std::cerr << "Error opening file for writing" << std::endl;
		}
	}

	// 清理
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	return 0;
}

/**
 * @brief 解析 Fit 文件数据
 *
 * 从指定路径中解析 FitData 数据，并将其返回。
 *
 * @param path 数据路径
 *
 * @return 解析后的 FitData 数据（结构体，含有以下字段的vector）
 *
 * timestamp; altitude_m; distance_m; grade_percent; temperature_c;
 *
 * speed_mps;
 *
 * power_w; cadence; heartrate;
 *
 * predicted_power_w;
 */
FitData parseJsonData(const std::string& path) {
	// InterpidsportFitService(path);

	std::ifstream file(path);
	rapidjson::IStreamWrapper isw(file);
	rapidjson::Document doc;
	doc.ParseStream(isw);

	FitData data;
	const int default_int = 0;
	const float default_float = 0.0f;

	if (doc.HasMember("result") && doc["result"].HasMember("records")) {
		const rapidjson::Value& records = doc["result"]["records"];
		for (rapidjson::SizeType i = 0; i < records.Size(); ++i) {
			const rapidjson::Value& record = records[i];

			data.timestamp.push_back(record.HasMember("timestamp") ? record["timestamp"].GetInt() : default_int);
			data.altitude_m.push_back(record.HasMember("altitude") ? record["altitude"].GetFloat() : default_float);
			data.distance_m.push_back(record.HasMember("distance") ? record["distance"].GetFloat() : default_float);
			data.gradient_percent.push_back(record.HasMember("grade") ? record["grade"].GetFloat() : default_float);
			data.temperature_c.push_back(record.HasMember("temperature") ? record["temperature"].GetInt() : default_int);

			data.speed_mps.push_back(record.HasMember("speed") ? record["speed"].GetFloat() : default_float);

			data.power_w.push_back(record.HasMember("power") ? record["power"].GetInt() : default_int);
			data.cadence.push_back(record.HasMember("cad") ? record["cad"].GetInt() : default_int);
			data.heartrate.push_back(record.HasMember("heartrate") ? record["heartrate"].GetInt() : default_int);
		}
	}

	return data;
}
