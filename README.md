# Tema 2 SO

by Andrei Mărunțiș

## Detalii despre implementare

Implementarea este o librarie de sincronizare a thread-urilor unui proces in user-space printr-o modalitate round robin (care ia in seama prioritatea proceselor si timpul cat acestea au rulat). Libraria expune un api cu mai multe functii explicate la link-ul ocw unde este postata tema.

Pentru a stoca datele despre thread-urile care ruleaza la un moment dat folosesc structuri definite de mine care contin informatiile relevante despre acele thread-uri: tid-ul, prioritatea, handler-ul specifice fiecarui proces. In plus, ca *mecanism de sincronizare* folosesc **semafoare**, astfel: asociez fiecarui thread cate un semafor (in structura descrisa mai sus) si il pun sa astepte la acel semafor dupa fiecare operatie executata de acel thread: fork, exec, wait, signal. Exceptie face firul principal de executie, care va rula neintrerupt pana aproape de final, din urmatorul motiv: pentru a crea thread-uri noi foi folosesc functia POSIX `pthread_create` care are nevoie sa i se elibereze resursele prin apelul functiei `pthread_join` din unul din thread-urile parinte ale acelui thread.

O structura importanta care ma ajuta in procesul de sincronizare este coada de prioritati, in care stau toate thread-urile care sunt pregatite sa ruleze pe procesor pana le vine randul. Implementarea cozii de prioritati foloseste mai multe cozi simple (implementate folosind liste simplu inlantuite), cate o coada pentru fiecare prioritate (6 in total). Astfel, operatiile asupra cozii de prioritati sunt foarte simple si eficiente ca timp (deoarece niciodata nu e nevoie sa parcurg listele, avand referinte catre primul si ultimul nod in orice moment) si putin ineficiente ca memorie.

Pentru a avea acces la thread-urile care ruleaza sau asteapta sa ruleze folosesc mai multe structuri de date:

- coada de prioritati descrisa mai sus
- o lista simplu inlantuita pentru thread-urile blocate
- un array static pentru a retine toate thread-urile in scopul eliberarii memoriei la finalul executiei

Sincronizarea efectiva o realizez prin functia `schedule` care verifica mai multe situatii, descrise si prin comentarii in cod:

- daca exista vreun thread care ruleaza
- daca exista vreun proces care asteapta sa ruleze
- daca procesul trebuie preemptat, fiindca:
    - i-a expirat cuanta de timp
    - a aparut alt proces cu prioritate mai mare decat el

La intrarea in functia schedule, toate thread-urile le consider blocate (asezate la semafor); in realitate procesul curent nu asteapta la semafor, dar se va aseza imediat dupa ce termina de executat functia `schedule`. Astfel, functia `schedule` imi alege thread-ul care ar trebui sa ruleze urmatorul pe procesor si ii incrementeaza semaforul (ii da efectiv voie sa ruleze).

## Ce am invatat din aceasta tema

- am aprofundat cunostiintele despre sincronizarea thread-urilor
- am invatat diferenta thread-proces
- am lucrat cu api-ul POSIX de lucru cu procese
- am lucrat cu semafoare
- am implementat pentru prima data o coada de prioritati

## Sugestii de imbunatatire a temei

Tema mi s-a parut deosebit de misto, insa are anumite defecte care o fac frustranta pentru cei care o rezolva. Unele probleme pe care le-am gasit si imbunatatiri pe care le sugerez le prezint mai departe:

- **explicarea cerintei lasa de dorit**: dupa ce am terminat de citit cerinta nu imi era deloc clar ce ar trebui sa fac, de unde ar trebui sa incep, fiindca nu intelegeam cum se doreste ca mecanismul pe care il implementez sa interactioneze cu exteriorul
    - in acest caz, exteriorul este checker-ul si cred ca oarecare detalieri despre modul sau de functionare (documentatie) ar ajuta foarte mult in rezolvarea acestei probleme
    - precizari pe care le-as gasi relevante si utile:
        - handler-urile noilor thread-uri vor apela inapoi functiile din api-ul construit de mine, iar numarul acestor apeluri este metrica (cuanta de timp) dupa care decid cat timp ruleaza un thread; nu am acces si nici nu ma intereseaza alte instructiuni in afara apelurilor de functii din libraria mea
        - cateva explicatii despre fisierele de test (functiile care vor rula pe thread-uri)
        - un exemplu parcurs **pe cod**
    - in fisierele unde se afla functiile de test si pe consola la rularea checker-ului apar numere diferite (testul 0 pe consola este testul 1 in input etc.)
    - laboratoarele de anul asta vizau exclusiv mecanisme de sincronizare in python, iar tema este in C :trollface:

### Closing remarks

Materia de SO este de departe cea mai interesanta si misto din semestrul acesta; de altfel, am participat la hackathon-ul de SO si mi-a placut la nebunie ceea ce am codat. De asemenea, cerintele erau explicate clar si concis, cu simt de raspundere, ca dovada ca se poate ca cerintele la aceasta materie sa fie clare si temele, placute.