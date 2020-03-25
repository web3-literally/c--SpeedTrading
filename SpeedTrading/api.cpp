
#include "api.h"

#include "constants.h"
#include "common.h"
#include "logger.h"
#include "structs.h"

#include <curl/curl.h>
#include <string>

using namespace std;

size_t write_data(void *contents, size_t size, size_t nmemb, void *userp) {
	((string *)userp)->append((char *)contents, size * nmemb);
	
	/*size_t datalen = size * nmemb;
	char *buf = (char *)malloc(datalen + sizeof(char));
	memset(buf, '\0', datalen);

	if (datalen) {
		memcpy(buf, ptr, datalen);
	}

	*userp = char2wstring(buf);
	free(buf);*/
	return size * nmemb;
}

size_t read_data(void *dest, size_t size, size_t nmemb, void *userp) {
	//NETDATA *data = (NETDATA *)userp;
	//size_t buffer_size = size * nmemb;

	//if (data->datalen) {
	//	/* copy as much as possible from the source to the destination */
	//	size_t copy_this_much = data->datalen;
	//	if (copy_this_much > buffer_size)
	//		copy_this_much = buffer_size;
	//	memcpy(dest, data->dataptr, copy_this_much);

	//	data->dataptr += copy_this_much;
	//	data->datalen -= copy_this_much;
	//	return copy_this_much; /* we copied this many bytes */
	//}

	return 0; /* no more data left to deliver */
}

int do_post_request(const string suburl, const wstring data)
{
	CURL *curl;
	CURLcode res;
	string response;
	string url = format2str("%s%s", API_URL.c_str(), suburl.c_str());

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");

		struct curl_slist *headers = NULL; // init to NULL is important 
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "charset: utf-8");
		curl_easy_setopt(curl, CURLOPT_HEADER, headers);

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, wchar2string(data.c_str()).c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK) {
			wstring err = char2wstring(curl_easy_strerror(res));
			log_error(_T("curl_easy_perform() failed: %s\n"), err.c_str());
		}
		else {
			char *ct;
			res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
			if ((CURLE_OK == res) && ct) {
				wstring w_response = char2wstring(extractData(response).c_str());
				log_inform(_T("response %s\n"), w_response.c_str());
			}
			else {
				wstring err = char2wstring(curl_easy_strerror(res));
				log_error(_T("curl_easy_getinfo() failed: %s\n"), err.c_str());
			}
		}

		/* always cleanup */
		curl_slist_free_all(headers);
	}
	
	/* always cleanup */
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return -1;
}

int do_multi_post_request(const wstring suburl, const void *param)
{
	CURL *curl;
	CURLcode res;
	TCHAR url[MAX_LENGTH];
	wstring response;
	_stprintf(url, _T("%s%s"), API_URL.c_str(), suburl.c_str());

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_URL, url);

		struct curl_slist *headers = NULL; // init to NULL is important 
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "charset: utf-8");
		curl_easy_setopt(curl, CURLOPT_HEADER, headers);

		//NETDATA response;

		///* size of the POST data */
		//if (param->datalen < 2 * 1024 * 1024 * 1024) {
		//	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, param->datalen);
		//}
		//else {		/* If size is larger than 2G, set this option */
		//	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, param->datalen);
		//}

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_data);
		curl_easy_setopt(curl, CURLOPT_READDATA, param);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK) {
			wstring err = char2wstring(curl_easy_strerror(res));
			log_error(_T("curl_easy_perform() failed: %s\n"), err.c_str());
		}
		else {
			TCHAR *ct;
			res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
			if ((CURLE_OK == res) && ct) {
				log_inform(response.c_str());
			}
			else {
				wstring err = char2wstring(curl_easy_strerror(res));
				log_error(_T("curl_easy_getinfo() failed: %s\n"), err.c_str());
			}
		}

		/* always cleanup */
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();

	return -1;
}

int do_get_request(const wstring suburl, const void *param)
{
	CURL *curl;
	CURLcode res;
	string response;
	string url = format2str("%s%s", API_URL.c_str(), suburl.c_str());

	curl = curl_easy_init();

	/*NETDATA response;
	TCHAR url[MAX_LENGTH];

	_stprintf(url, _T("%s%s"), API_URL, suburl);
	if (param->dataptr) {
		_tcscat(url, _T("?"));
		_tcscat(url, param->dataptr);
	}*/

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");

		struct curl_slist *headers = NULL; // init to NULL is important
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "charset: utf-8");
		curl_easy_setopt(curl, CURLOPT_HEADER, headers);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK) {
			wstring err = char2wstring(curl_easy_strerror(res));
			log_error(_T("curl_easy_perform() failed: %s\n"), err.c_str());
		}
		else {
			char *ct;
			res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
			if ((CURLE_OK == res) && ct) {
				wstring w_response = char2wstring(extractData(response).c_str());
				log_inform(_T("response %s\n"), w_response.c_str());
			}
			else {
				wstring err = char2wstring(curl_easy_strerror(res));
				log_error(_T("curl_easy_getinfo() failed: %s\n"), err.c_str());
			}
		}

		/* always cleanup */
		curl_slist_free_all(headers);
	}

	/* always cleanup */
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return -1;
}

