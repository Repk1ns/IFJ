# IFJ

Lexikalni analyza = 1. commit
  nefunguji cisla a string ma taky problemy, ale vim kde je chyba a opravim to asap hned jak bude cas
  dodam stejne tak i komentare a dokumentaci
  2. commit = opraveny cisla, stringy, jediny eof zlobi, zaznamenala jsem to ted pred par minutami, nicmene vse ostatni by melo fungovat uz;
  instrukce tam tedy nepatri, hodila jsem vam je do mainu do komentare abyste je pouzili potom dale a nemuseli je psat znova :)
  !! PROSIM, kdyztak si opravte nazev, mela jsem puvodne napsano lexical_analyse, ale je to spatne, spravne je lexical_analysis, tak at vam to nezlobi

SymTable = 2. commit 
  -> Upraveny Makefile s CFLAGMI kvoli errorom pri preklade
  -> Pridany hlavickovy subor pre tabulku ->TODO dorobit a implementovat vsetky funkcie
  -> Vytvorenie suborov pre syntakticku analyzu kvoli tabulke
  -> Pridany gitignore
  -> pridane komentare pre puzzlebean do lexikalnej analyzy -> v poli charov nepridavas "\0" na koniec, preto su errory, a bol tam nejaky neosetreny stav(je tam komentar)
  
