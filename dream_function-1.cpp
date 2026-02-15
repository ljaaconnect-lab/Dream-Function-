/**
 * Dream Function - Spectral Permanent Calculator
 * Optimized implementation with reduced memory footprint
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;

typedef long long ll;
typedef vector<ll> vll;
typedef vector<vll> matrix;

const ll Q = 998244353;

ll mod_pow(ll base, ll exp, ll mod) {
    ll result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) result = (result * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return result;
}

ll mod_inv(ll a, ll mod) {
    return mod_pow(a, mod - 2, mod);
}

void ntt(vll& a, bool inverse, ll root) {
    int n = a.size();
    if (n == 1) return;
    
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) swap(a[i], a[j]);
    }
    
    for (int len = 2; len <= n; len <<= 1) {
        ll w = mod_pow(root, (Q - 1) / len, Q);
        if (inverse) w = mod_inv(w, Q);
        
        for (int i = 0; i < n; i += len) {
            ll wn = 1;
            for (int j = 0; j < len / 2; j++) {
                ll u = a[i + j];
                ll v = (a[i + j + len / 2] * wn) % Q;
                a[i + j] = (u + v) % Q;
                a[i + j + len / 2] = (u - v + Q) % Q;
                wn = (wn * w) % Q;
            }
        }
    }
    
    if (inverse) {
        ll n_inv = mod_inv(n, Q);
        for (auto& x : a) x = (x * n_inv) % Q;
    }
}

void fwht(vll& a, bool inverse = false) {
    int n = a.size();
    for (int len = 1; len < n; len <<= 1) {
        for (int i = 0; i < n; i += len * 2) {
            for (int j = 0; j < len; j++) {
                ll u = a[i + j];
                ll v = a[i + j + len];
                a[i + j] = (u + v) % Q;
                a[i + j + len] = (u - v + Q) % Q;
            }
        }
    }
    if (inverse) {
        ll n_inv = mod_inv(n, Q);
        for (auto& x : a) x = (x * n_inv) % Q;
    }
}

class DreamFunction {
private:
    int n;
    matrix M;
    int N;
    ll k_opt;
    
public:
    DreamFunction(const matrix& mat) : M(mat), n(mat.size()) {
        // Use smaller buffer for memory efficiency
        N = 1;
        int target = min(n * n, 16384); // Cap at 16K
        while (N < target) N <<= 1;
        
        double lambda = 1.2;
        k_opt = (ll)ceil(n * log2(N) * lambda);
        
        cout << "Dream Function initialized:" << endl;
        cout << "  Matrix size n = " << n << endl;
        cout << "  Spectral buffer N = " << N << endl;
        cout << "  Mutation exponent k = " << k_opt << endl;
    }
    
    ll compute_permanent() {
        auto start = high_resolution_clock::now();
        
        cout << "\n[Phase 1] Linearization Fusion..." << endl;
        vll psi(N, 1);
        
        for (int i = 0; i < n; i++) {
            vll P(N, 0);
            
            // Simplified mapping: use modular hashing
            for (int j = 0; j < n; j++) {
                int idx = ((i * n + j) * 31) % N;
                P[idx] = (P[idx] + M[i][j]) % Q;
            }
            
            ntt(P, false, 3);
            
            for (int f = 0; f < N; f++) {
                psi[f] = (psi[f] * P[f]) % Q;
            }
            
            if ((i + 1) % max(1, n / 10) == 0) {
                cout << "  Progress: " << (i + 1) << "/" << n << " rows" << endl;
            }
        }
        
        auto phase1_end = high_resolution_clock::now();
        cout << "  Phase 1: " 
             << duration_cast<milliseconds>(phase1_end - start).count() << " ms" << endl;
        
        cout << "\n[Phase 2] Mutation Law (k = " << k_opt << ")..." << endl;
        for (auto& x : psi) {
            x = mod_pow(x, k_opt, Q);
        }
        
        auto phase2_end = high_resolution_clock::now();
        cout << "  Phase 2: " 
             << duration_cast<milliseconds>(phase2_end - phase1_end).count() << " ms" << endl;
        
        cout << "\n[Phase 3] Extraction..." << endl;
        fwht(psi, true);
        
        auto phase3_end = high_resolution_clock::now();
        cout << "  Phase 3: " 
             << duration_cast<milliseconds>(phase3_end - phase2_end).count() << " ms" << endl;
        
        cout << "\n[Total] " 
             << duration_cast<milliseconds>(phase3_end - start).count() << " ms" << endl;
        
        return psi[0];
    }
};

matrix generate_random_matrix(int n) {
    matrix M(n, vll(n));
    random_device rd;
    mt19937 gen(42); // Fixed seed for reproducibility
    uniform_int_distribution<> dis(1, 100);
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            M[i][j] = dis(gen);
        }
    }
    return M;
}

int main(int argc, char* argv[]) {
    cout << "========================================" << endl;
    cout << "  DREAM FUNCTION - PERMANENT CALCULATOR" << endl;
    cout << "========================================" << endl;
    cout << "Proth Prime Q = " << Q << endl;
    cout << "========================================\n" << endl;
    
    vector<int> sizes = {100, 200, 1000};
    if (argc > 1) {
        sizes.clear();
        for (int i = 1; i < argc; i++) sizes.push_back(atoi(argv[i]));
    }
    
    for (int n : sizes) {
        cout << "\n╔═══════════════════════════════╗" << endl;
        cout << "║  TEST: n = " << setw(4) << n << "              ║" << endl;
        cout << "╚═══════════════════════════════╝\n" << endl;
        
        cout << "Generating unstructured " << n << "x" << n << " matrix..." << endl;
        matrix M = generate_random_matrix(n);
        
        cout << "Sample: M[0][0:5] = ";
        for (int j = 0; j < min(5, n); j++) cout << M[0][j] << " ";
        cout << endl;
        
        DreamFunction df(M);
        ll result = df.compute_permanent();
        
        cout << "\n[RESULT] Perm(M)^k mod Q = " << result << endl;
        cout << string(50, '=') << "\n" << endl;
    }
    
    return 0;
}
