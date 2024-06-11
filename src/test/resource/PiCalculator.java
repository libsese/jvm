class PiCalculator {
    public static void main(String[] args) {
        // 设置迭代次数
        int iterations = 100000;
        double pi = calculatePi(iterations);
    }

    public static double calculatePi(int iterations) {
        double pi = 0.0;

        // 使用莱布尼茨级数计算 π
        for (int k = 0; k < iterations; k++) {
            // 计算每一项的值
            double term = 1.0 / (2 * k + 1);

            // 根据 k 的奇偶性添加或减去该项
            if (k % 2 == 0) {
                pi += term;
            } else {
                pi -= term;
            }
        }

        // 最后乘以 4 得到 π 的值
        pi *= 4;
        return pi;
    }
}