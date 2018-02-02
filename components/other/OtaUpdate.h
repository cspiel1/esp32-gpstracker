#ifndef OTA_UPDATE
#define OTA_UPDATE

#include <esp_ota_ops.h>

#define BUFFSIZE 1024

class OtaUpdate {

	const char* _serverip;
	int _serverport;
	const char* _filename;

	int socket_id;
	int binary_file_length;
	/*an ota data write buffer ready to write to the flash*/
	char ota_write_data[BUFFSIZE + 1];

	static void ota_task(void *pvParameter);
	bool read_past_http_header(char text[], int total_len,
			esp_ota_handle_t update_handle);
	void run_ota();
	void __attribute__((noreturn)) task_fatal_error();
	int read_until(char *buffer, char delim, int len);
	bool connect_to_http_server();

	public:
		OtaUpdate(const char* serverip, int serverport);
		~OtaUpdate();

		void setFileName(const char* file);

		void start();
};

#endif
