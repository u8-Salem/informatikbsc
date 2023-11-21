int f(int i) {
    printf("called f\n");
    if (i < 0) {
        return -i;
    } else {
        return 3 * i;
    }
}