- int *reverseArray(int array[], int size)
    - Inverse tous les éléments d'une array
    - Parallèle (For)


- struct table *allocateT(int size)
    - Alloue l'espace passé en paramètre pour une table
    - Non parallèle (malloc impossible à paralléliser)


- void displayTable(struct table *table)
    - Affiche une table sur la sortie standard (dev only)
    - Non parallèle (affichage impossible à paralléliser)


- void montee(struct table *nums, struct table *resultat)
    - Étape de la montée pour SUM uniquement
    - Parallèle (sur 2 For)
    (- idem pour montee_max())


- void descente(struct table *resultat_montee, struct table *resultat)
    - Étape de la descente pour SUM uniquement
    - Parallèle (For)
    (- idem pour descente_max())


- void fusion(struct table *resultat_montee, struct table *resultat_descente, struct table *resultat)
    - Fusionne les résultats de la montée et de la descente dans 1 seul tableau
    - Parallèle (For)
    (- idem pour fusion_max())


- struct table *prefixSum(int nums[], int size, int verbose)
    - Détermine PSUM
    - Non parallèle mais utilise montee, descente et fusion qui sont elles-mêmes parallèles + la fonction est appelé dans une section paralèlle distincte dans le main
    - Idem pour prefixMax(), suffixSum() et suffixMax()


- void maxSubSequence(int nums[], int size, struct table *PSUM, struct table *SSUM, struct table *PMAX, struct table *SMAX, /**int *resultat,**/ int VERBOSE)
    - Calcule et affiche la sous-séquence maximale finale à partir de PSUM, SSUM, PMAX et SMAX
    - Parallèle (For calcul de M, For trouvé le maximum)


- void displayResult(int nums[], int resultat[])
    - Affiche le resultat final (dev only)
    - Non parallèle (affichage non parallélisable)


- int main(int argc, char *argv[])
    - Parse l'input, lance PSUM et SSUM en parallèle puis PMAX et SMAX en parallèle également et enfin lance le calcul et l'affichage du résultat final (maxSubSequence())
    - "Parallèle" (utilise les fonctions qui sont elles-mêmes parallèles + lance 2 fois 2 sections en parallèle)