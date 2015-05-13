# Problème du voyageur de commerce
Version française traduite de : https://github.com/beckysag/traveling-salesman/blob/master/README.md
Implementation du TSP avec l'heuristique de Christofides

Présentation
-----------

**Problème**: Etant donné un ensemble de villes séparées par des distances données, trouver l'itinéraire le plus court passant par chaque ville une et une seule fois.

**Implémentation**: Utiliser l'heuristique de Christofides pour construire un cycle puis l'algorithme de recherche locale 2-opt pour l'améliorer.

Compilation
-----------
Dépendances : [CMake](http://www.cmake.org/), [boost-libs](http://www.boost.org/)
```
git clone https://github.com/melkir/TSP-Christofides.git
cd TSP-Christofides
mkdir build; cd build; cmake ../.; make
```

Utilisation
-----------

```
./TSP_Christofides fichier_entree
```
où fichier d'entrée est un fichier texte de format suivant:
```
0    x-coord    y-coord
1    x-coord    y-coord
.
.
.
n-1  x-coord    y-coord

```
et <i>n</i> est le nombre de villes

#### Résultats du programme
Un nouveau fichier texte (.tour) contenant n + 1 lignes, où n est le nombre de villes. La première ligne correspond à la longueur de la meilleur tournée trouvé. Les n lignes suivantes correspondent aux identifiants de chaque villes, dans l'ordre dans lesquelles elles sont visités par la tournée. Chaque ville n'apparait qu'une seule fois dans la liste.

Implémentation
-----------

### 1. L'algoritmhe de Christofides

Christofides' est une heuristique de construction de cycle qui peut être appliqué aux graphes avec la propriété suivante : un graphe complet  _G_ = ( _V_,_E_,_w_ ) avec des poids d'arcs qui satisfont l'inégalité du triangle _w_(_x,y_) + _w_(_y,z_) ≤ _w_(_x,z_).

Source : http://en.wikipedia.org/wiki/Christofides_algorithm

1. Trouver un arbre couvrant minimal T de G
2. Soit O l'ensemble des sommets avec un degré impair dans T
3. Trouver un couplage parfait M avec un poids minimum sur les sommets de O
4. Ajouter M à T pour obtenir multigraphe H
5. Trouver un cycle Eulérien de H
6. Convertir le cycle Eulérien en un chemin Hamiltonien en sautant les nœuds visités (shortcuts).

![alt text][fig1]

#### 1.1 Arbre couvrant minimun (MST)
J'ai utilisé l'algorithme de Prim pour trouver l'arbre couvrant minimum dans G:
```scilab
function MST(G = (V, E))
  for v in V do
    key[v] <-- infinity
    parent[v] <-- NULL
    insert v into Q
  end for
  key[0] <-- 0
  while !Q.empty() do
    v <-- Q.removeMin()
    for u adjacent to v do
      if u ∈ Q and weight(u, v) < key[u] then
        parent[u] <-- v
      end if
      key[v] <-- weight(u,v)
    end for
  end while
end function
```

L’algorithme traite un ensemble _Q_ contenant tous les sommets qui ne sont pas encore dans l’arbre (Initialement, _Q_ est vide). Le programme itère pour chaque sommet _v_ non encore présent dans l’arbre (utilisant le sommet 0 comme le sommet initial) et choisi l'arête de poids minimal _(u,v)_ où _u_ est un sommet déjà présent dans l’arbre. On obtient ainsi l'arête de poids minimun traversant le passage (puisqu’il relie une arête dans le MST avec une arête qui n’y est pas encore). Le sommet _v_ est ensuite ajouté dans l’arbre. L’algorithme continue jusqu’à ce que _Q_ soit vide de sorte que tous les sommets soient ajoutés dans le MST.

Source : http://fr.wikipedia.org/wiki/Algorithme_de_Prim

![alt text][fig2]

#### 1.2 Sommets avec un degré impair dans le MST
La prochaine étape consiste à trouver les sommets de degré impair dans le MST. Comme l'arbre est stocké dans une liste de  proximités avec des vecteurs C++, cette procédure nécessite uniquement de vérifier la taille des vecteurs pour chaque noeud dans la liste.

#### 1.3 Parfaite adéquation pondérée pour sommets impairs

Un graphe de connexe sans boucles a un _n_ arrête et _n-1_ degrés. Nous trouvons maintenant une correspondance parfaite de tous ces sommets de telle sorte que tous les sommets aient un degrés pair. Idéalement, nous devrions trouver une correspondance minimale, mais a la place, j’ai utilisé un algorithme glouton pour trouver une correspondance minimale approximative.

```scilab
function PerfectMatching()
  Input: odds (list of odd vertices), G (adjacency list)
  while !odds.empty do
    v <-- odds.popFront()
    length <-- ∞
    for u ∈ odds do
      if weight(u,v) < length then
        length <-- weight(u,v)
        closest <-- u
      end if
    end for
    G.addEdge(closest,u)
    odds.remove(closest)
  end while
end function
```

![alt text][fig3]

L'ensemble des sommets pairs sont maintenant ajoutés à notre MST, formant un nouveau multigraphe.

#### 1.4 Cycle Eulérien
Ensuite, nous parcourons le graphe de façon à créer un cycle eulérien. Pour commencer on choisi aléatoirement un noeud de notre multigraphe, si ce noeud à des voisins, on l'ajoute à une pile, on sélectionne un voisin, on supprime l'arrête qui les relies du graphe, puis on utilise ce voisin comme sommet courant. Si notre sommet n’a plus de voisin, nous l’ajoutons à notre parcours et enlevons le sommet en haut de la pile pour l’utiliser comme notre sommet courant. Nous continuons le tracé de notre parcours de cette façon jusqu’à ce que la pile soit vide et que le dernier sommet n’ai plus de voisin.

![alt text][fig4]


#### 1.5 Chemin Hamiltonien
Enfin, nous transformons notre cycle Eulérien en un chemin Hamiltonien en marchant le long de la tournée Euler, tout en vérifiant à chaque arrêt si ce nœud a déjà été visité. Si c'est le cas, nous ignorons ce nœud et passons au suivant. Comme notre graphe satisfait l'inégalité du triangle, court-circuité de cette façon les sommets n'augmentera pas la longueur de notre chemin.

![alt text][fig5]

### 2. Two-Opt
Après qu'une tournée ai été construite en utilisant l'heuristique Christofides, j'ai appliqué l'algorithme de recherche locale 2-opt pour optimiser le chemin. L’algorithme 2.opt examine chaque arête du parcours. Pour chaque arête, il regarde toutes les arêtes non adjacentes, et détermine si en enlevant ces deux arêtes puis en les remettant d'une autres façon cela raccourcirait le parcours. Si c’est le cas, les arêtes sont échangées. La recherche continue jusqu’à ce que le parcours ne puisse plus être optimisé de cette façon.

Source : http://fr.wikipedia.org/wiki/2-opt


[fig1]: https://github.com/melkir/TSP-Christofides/raw/master/images/figure01.png "Figure 1"
[fig2]: https://github.com/melkir/TSP-Christofides/raw/master/images/figure02.png "Figure 2"
[fig3]: https://github.com/melkir/TSP-Christofides/raw/master/images/figure03.png "Figure 3"
[fig4]: https://github.com/melkir/TSP-Christofides/raw/master/images/figure04.png "Figure 4"
[fig5]: https://github.com/melkir/TSP-Christofides/raw/master/images/figure05.png "Figure 5"
