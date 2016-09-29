//转小写
void lower_string(char s[]) {
  int c = 0;
 
  while (s[c] != '\0') {
    if (s[c] >= 'A' && s[c] <= 'Z') {
      s[c] = s[c] + 32;
    }
    c++;
  }
}

//转大写
void upper_string(char s[]) {
  int c = 0;
 
  while (s[c] != '\0') {
    if (s[c] >= 'a' && s[c] <= 'z') {
      s[c] = s[c] - 32;
    }
    c++;
  }
}
