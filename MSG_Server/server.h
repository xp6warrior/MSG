/*
    Fetches private IP address of local machine, places it in buffer
    @param *buff ptr to buffer
    @param buff_len size of buffer
    @return -1 if fail
*/
int getDefaultIP(char *buff, int buff_len);

int getIPintegrity();