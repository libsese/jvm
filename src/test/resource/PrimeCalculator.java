public class PrimeCalculator {

    public static void main(String[] args) {
        int n = 50;
        int primeNumber = findNthPrime(n);
    }

    public static int findNthPrime(int n) {
        int count = 0;
        int num = 1;

        while (count < n) {
            num++;
            if (isPrime(num)) {
                count++;
            }
        }

        return num;
    }

    public static boolean isPrime(int num) {
        if (num <= 1) {
            return false;
        }
        if (num == 2) {
            return true;
        }
        if (num % 2 == 0) {
            return false;
        }

        int sqrt = manualSqrt(num);
        for (int i = 3; i <= sqrt; i += 2) {
            if (num % i == 0) {
                return false;
            }
        }
        return true;
    }

    public static int manualSqrt(int num) {
        int left = 1, right = num;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (mid <= num / mid) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return right;
    }
}
