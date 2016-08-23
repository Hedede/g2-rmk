//zMAT4
class zMAT4 {
    /* die Matrix hat folgende Gestalt:
     *
     *  x1  y1  z1  p1
     *  x2  y2  z2  p2
     *  x3  y3  z3  p3
     *
     * Dabei sind x, y, z die Bilder der Einheitsvektoren unter der Transformation.
     * Andersgesagt: x, y, z sind die Vektoren, die im Spacer angezeigt werden, wenn man das
     * Vob anklickt (davon ist y die Komponente die normalerweise nach oben zeigt).
     * Ist ein Punkt im lokalen Koordinatensystem des Vobs an (a1, a2, a3), und liegt das
     * Vob mit Transformationsmatrix M in der Welt dann ist der Punkt im
     * Koordinatensystem der Welt an M * (a1, a2, a3, 1)^T.
     * ("^T" soll nur bedeuten, dass der Vektor eigentlich senkrecht stehen muss).
     * Die vierte Zeile der Matrix ist ungenutzt und sinnlos. */

    /* Zeilenweise */
    zREAL v0[4];
    zREAL v1[4];
    zREAL v2[4];
    zREAL v3[4];
};

zMAT4 Alg_Identity3D()
{
	return {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
}
