# cdn-dns

Proc jsem si zvolil tento projekt?

- Mám rad praci s daty, kde je musim tridit pripadne je efektivne skladovat

Jake reseni jsem si vybral?

- Jako datovou strukturu jsem zvolil Trie - struktura v ktere se vyhledava v O(p) case podle prefixu v mem pripade s
  ipv6 O(128) takze v konstatnim.
- Další možnost co jsem našel Compressed trie. Neudělal jsem ho, protože je složitější na implementaci a na tenhle
  úkol mi stačil jednoduchý bit trie. Do produkce pro větší škálovatelnost bych to předělal, protože má míň uzlů ->
  menší paměť a rychlejší průchod.

Problemy na ktere jsem narazil:

- Chvíli jsem bojoval, kvůli sve nepozornosti, s tím podle čeho rozdělit řadit children v nodech, začínal jsem na tom
  že jsem to dělal podle hexa čísel, nakonec mi došlo že prefix length jde po bitech a ne po nibblech.
- Ještě predtím než jsem přešel na jednotlivé bity, tak jsem si chvíli uchovával hodnotu v nodu - po chvili mi doslo ze
  to je redundatní tím, že se indexuje podle hodnoty v poli a zabíralo to tak zbytečne místo navíc
- V jednu chvíli jsem checkoval jestli insert proběhl správně a až po validici jsem předával data to Trie, po chvíli mi
  došlo že tenhle check je redundatní, protože mám invariant checky v Ipv6Net a jediný scénář kdy mi insert může selhat
  je právě tehdy když uz je insertnutý stejný prefix jako ten co bych vkládal.
- Můj find nenašel prefix i když měl, bylo to způsobeno špatnou podmínkou která vracela hodnotu pouze tehdy když Node
  neměl už žádné další children, což se nemusí vždy stát.
-

Co by šlo zlepšit

- Odstranit závislost na Ipv6Net, a udělat z toho čisty bit Trie, aby to šlo použít například i s ipv4 adresami.
- Přidat možnost načítat routing dat z více zdrojů - abstraktní třída Source který má metodu get() a z něj dědí různé
  zdroje.
- Přidat možnost mazání z trie.
- Odstranit shared pointery a ušetrit místo - nahradit případně raw nebo unique pointery.
  Bezpečnost

- Jak jsem již zmiňoval přidal jsem invariant checky do třídy Ipv6Net, takže by nemělo dojít k použití nevalidních dat a
  rozbití systému.
- Vše co vracím pomocí getterů je read only, takže ani zde nemůže dojít k rozbití.

Time spent

- cca 5h