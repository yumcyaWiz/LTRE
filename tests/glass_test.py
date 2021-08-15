import numpy as np
from numpy import linalg


def refract(v: np.ndarray, n: np.ndarray, iorI: float, iorT: float) -> np.ndarray:
    t_h = -iorI / iorT * (v - np.dot(v, n) * n)
    if np.linalg.norm(t_h)**2 > 1:
        return None
    t_p = -np.sqrt(1 - np.linalg.norm(t_h)**2) * n
    return t_h + t_p


def normalize(v: np.ndarray) -> np.ndarray:
    return v / np.linalg.norm(v)


if __name__ == "__main__":
    v = normalize(np.array([-1000, 1, 0]))
    n = normalize(np.array([-100, 1, 0]))
    iorI = 1.0
    iorT = 1.5

    t = refract(v, n, iorI, iorT)
    h = normalize(-(iorI * v + iorT * t))
    print(t)
    print(h)
