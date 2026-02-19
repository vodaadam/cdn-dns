Proč jsem si zvolil tento projekt?

- Mám rád práci s daty, kde je musím třídit, případně je efektivně skladovat.

Jaké řesení jsem si vybral?

- Jako datovou strukturu jsem zvolil trie – strukturu, ve které se vyhledává v čase O(p) podle délky prefixu. V mém
  případě pro IPv6 je p ≤ 128, takže je to v praxi konstantní.
- Další možnost co jsem našel Compressed trie. Neudělal jsem ho, protože je složitější na implementaci a na tenhle
  úkol mi stačil jednoduchý bit trie. Do produkce pro větší škálovatelnost bych to předělal, protože má míň uzlů ->
  menší paměť a rychlejší průchod.

Problémy na které jsem narazil:

- Jak si uchovávat children v Nodu. Nakonec jsem udělal statické pole o 2 prvcích, ale pokud bych musel pracovat např.
  se slovy, tak bych vytvořil bit masku s vectorem.
- V jednu chvíli jsem checkoval jestli insert proběhl správně a až po validici jsem předával data to Trie, po chvíli mi
  došlo že tenhle check je redundatní, protože mám invariant checky v Ipv6Net a jediný scénář kdy mi insert může selhat
  je právě tehdy když uz je insertnutý stejný prefix jako ten co bych vkládal.
- Můj find nenašel prefix i když měl, bylo to způsobeno špatnou podmínkou která vracela hodnotu pouze tehdy když Node
  neměl už žádné další children, což se nemusí vždy stát.

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