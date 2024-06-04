import java.io.*;

class Hello {
    public static void main(String args[]) throws IOException {
        Hello nullable = null;
        Hello hello = new Hello();
        hello.test1(args[0], 20);
        throw new IOException();
    }

    public int test1(String str, long number) {
        return 0;
    }

    private int a;
    public int b;
    protected int c;
};
