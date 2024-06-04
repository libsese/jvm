import java.io.*;

class Hello {
    public static void main(String args[])  {
        Hello nullable = null;
        Hello hello = new Hello();
        try {
            hello.test1(args[0], 20);
        } catch (IOException e) {
            hello.bool = true;
        } finally {
            hello = null;
        }
    }

    public int test1(String str, long number) throws IOException {
        return 0;
    }

    public int test2(Object obj[][]) {
        return 1;
    }

    public int test3(int i, long j, double d) {
        return 0;
    }

    boolean bool = false;
    private int a;
    public int b;
    protected int c;
};
