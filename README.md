# Problème du voyageur de commerce
Implementation du TSP avec l'heuristique de Christofides

Présentation
-----------

Problème: Etant donné un ensemble de villes séparées par des distances données, trouver l'itinéraire le plus court passant par chaque ville une et une seule fois.

Implementation: Utiliser l'heuristique de Christofides pour construire un cycle puis l'algorithme de recherche locale 2-opt pour l'améliorer.


Utilisation
-----------

```
./tsp fichier_entree
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

#### Sortie du programme
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

L'étape d'initialisation consiste à choisir un sommet quelconque du graphe initial. Au bout de la première étape, on se retrouve ainsi avec un arbre contenant 1 sommet et 0 arête. Ensuite, on construit récursivement l'arbre minimal de la façon suivante : à l'étape n, ayant déjà construit un arbre contenant n sommets et n-1 arêtes, on établit la liste de toutes les arêtes reliant un sommet de l'arbre à un sommet qui n'est pas sur l'arbre. On choisit alors une arête de poids minimal, que l'on rajoute à l'arbre ; l'arbre contient à présent n+1 sommets et n arêtes. L'algorithme se termine lorsque tous les sommets du graphe sont contenus dans l'arbre.

Source : http://fr.wikipedia.org/wiki/Algorithme_de_Prim

![alt text][fig2]

#### 1.2 Sommets avec un degré impair dans le MST
La prochaine étape consiste à trouver les sommets de degré impair dans le MST. Comme l'arbre est stocké dans une liste de  proximités avec des vecteurs C++, cette procédure nécessite uniquement de vérifier la taille des vecteurs pour chaque noeud dans la liste.

#### 1.3 Parfaite adéquation pondérée pour sommets impairs

Nous constatons maintenant un couplage parfait entre ces sommets afin que tous les sommets aient le même degré. Idéalement, nous aimerions trouver une correspondance minimale, mais dans le cas présent j'ai utilisé un algorithme glouton pour trouver une correspondance minimale approximative.

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

L'ensemble des sommets pair est maintenant ajouté à notre MST, formant un nouveau multigraphe.

#### 1.4 Cycle Eulérien
Ensuite, nous parcourons le graphe de façon à créer un cycle eulérien. Pour commencer on choisi aléatoirement un noeud de notre multigraphe, si ce noeud à des voisins, on l'ajoute à une pile, on sélectionne un voisin, on supprime l'arrête qui les relies du graphe, puis on utilise ce voisin comme sommet courant. Si notre sommet n'a pas de voisins à gauche, nous l'ajoutons à notre cycle et l'enlevons de la pile afin de l'utiliser comme sommet courrant. On continue à tracer le cycle de cette façon jusqu'à qu'a ce que la pile soit vide et que le dernier sommet n'ai plus de voisins à gauche.

![alt text][fig4]


#### 1.5 Chemin Hamiltonien
Enfin, nous transformons notre cycle Eulérien en un chemin Hamiltonien en marchant le long de la tournée Euler, tout en vérifiant à chaque arrêt si ce nœud a déjà été visité. Si c'est le cas, nous ignorons ce nœud et passons au suivant. Comme notre graphe satisfait à l'inégalité du triangle, court-circuité les sommets de cette manière n'augmentera pas la longueur de notre chemin.

![alt text][fig5]

### 2. Two-Opt
Après qu'une tournée ai été construite en utilisant l'heuristique Christofides, j'ai appliqué l'algorithme de recherche locale 2-opt pour optimiser le chemin. A chaque étape, on supprime deux arêtes de la solution courante et on reconnecte les deux tours ainsi formés. Cette méthode permet, entre autres, d'améliorer le coût des solutions en supprimant les arêtes sécantes lorsque l'inégalité triangulaire est respectée.

Source : http://fr.wikipedia.org/wiki/2-opt


[fig1]: https://github.com/melkir/TSP-Christofides/raw/master/images/figure01.png "Figure 1"
[fig2]: https://github.com/melkir/TSP-Christofides/raw/master/images/figure02.png "Figure 2"
[fig3]: https://github.com/melkir/TSP-Christofides/raw/master/images/figure03.png "Figure 3"
[fig4]: https://github.com/melkir/TSP-Christofides/raw/master/images/figure04.png "Figure 4"
[fig5]: https://github.com/melkir/TSP-Christofides/raw/master/images/figure05.png "Figure 5"
