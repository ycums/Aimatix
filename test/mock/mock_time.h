unsigned long mockMillis = 0; 
unsigned long millis() { return mockMillis; }
void setMockTime(unsigned long time) { mockMillis = time; }
void resetMockTime() { mockMillis = 0; }
