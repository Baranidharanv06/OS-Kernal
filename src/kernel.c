extern void exceptions_init();
extern void shell_main();

void kernel_main() {
    exceptions_init();
    shell_main();
}
