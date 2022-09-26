from math import ceil, floor
from scipy.special import comb
from pprint import pprint

def runAnalysis(flows, nodes):
    def pdr(d):
        if d < 1.5:
            return 0.99
        else:
            return 0.99 / (1 + ((1/3) * (d - 1.5))**2)


    def S(x):
        return 1 + x*nodes

    def binF(s, pdr, conf):
        m = 0
        comp = -1
        if conf < 0 or conf > 1.0:
            raise ValueError("Inv Conf")
        while comp < conf:
            m += 1
            comp = 0
            for k in range(m):
                comp += comb(s, k, True) * ((1-(pdr**2)) ** k) * ((pdr**2) ** (s-k))
        return m-1

    def Fm(L, s):
        if L == "LO":
            #return 0
            return binF(s, pdr(2.0), 0.999)#5 #2 + ceil(s / 10)
        elif L == "HI":
            #return 0
            return max(Fm("LO", s), binF(s, pdr(3.0), 0.99999))
            #return 7 + ceil(s / 5)
        else:
            raise ValueError("CL")


    def XLo(i, start=1):
        X = 1 + Fm("LO", start)
        for flow in flows[0:i]:
            X += ceil(S(start) / flow["period"])
        if X == start:
            return X
        if S(X) >= flows[i]["period"]:
            raise ValueError("STOP")
        else:
            return XLo(i, X)

    def XHi(i, start=1):
        X = 1 + Fm("HI", start)
        for flow in flows[0:i]:
            if flow["criticality"] == "HI":
                X += ceil(S(start) / flow["period"])
            elif flow["criticality"] == "LO":
                X += ceil(flows[i]["RLo"] / flow["period"])
            else:
                raise ValueError("CL")
        if X == start:
            return X
        if S(X) >= flows[i]["period"]:
            raise ValueError("STOP")
        else:
            return XHi(i, X)

    def R(i, L):
        if L == "LO":
            x = XLo(i)
            #print(i, L, x, Fm(L, x))
            return S(x)
        elif L == "HI":
            x = XHi(i)
            #print(i, L, x, Fm(L, x))
            return S(x)
        else:
            raise ValueError("CL")

    for i in range(len(flows)):
        flows[i]["RLo"] = R(i, "LO")
        if flows[i]["criticality"] == "HI":
            flows[i]["RHi"] = R(i, "HI")
        else:
            flows[i]["RHi"] = -1
