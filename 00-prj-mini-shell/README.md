# MySH - Shell Linux minimal

L’appréhension de l’opération de « parsing » d’une ligne de commande a donné lieu à une réflexion sur la façon d’organiser l’ensemble des informations que l’on pouvait en sortir. Ainsi, il a été choisi que chaque nouvelle analyse d’une ligne de commande génère une construction hiérarchisée d’une structure d’objets à multiples étages.

## Principe des CmdOper, CmdRedir et CmdArgs
Le principe est d’abord d’extraire des « commandes opérateurs » (nommées « CmdOper »), puis ensuite des « commandes redirecteurs » (nommées « CmdRedir »), et enfin des « arguments de commande » (nommés « CmdArgs »).

Les séparateurs de commandes opérateurs (CmdOper) sont les opérateurs : « && », « || », « & ».
Les séparateurs de commandes redirecteurs (CmdRedir) sont les redirecteurs : « | », « > », « >> », « < », « << ».
Les séparateurs de CmdArgs sont les espaces (à noter que les simples et doubles quotes sont bien interprétés).

Depuis une ligne de commande, on extrait des CmdOper, et plus précisément une liste chaînée de CmdOper reliés par des opérateurs (&&, ||, &). Puis pour chaque CmdOper trouvé, on en extrait des CmdRedir, et plus précisément une liste chaînée de CmdRedir reliés par des redirecteurs (|, >, >>, <, <<). Enfin, pour chaque CmdRedir trouvé, on en extrait des CmdArgs contenus dans la sous-commande associée.

## Parseurs CmdOper, CmdRedir et CmdArgs

Le parseur CmdOper prend en entrée une ligne de commande. Il recherche les opérateurs « && », « || » ou « & ». Puis il construit petit à petit une liste chaînée de CmdOper reliés les uns avec les autres. Chaque CmdOper dispose d’un certain nombre d’informations dont le type d’opérateur qui le relie au CmdOper suivant.

Le parseur CmdRedir prend en entrée une chaine de caractère qui correspond à un CmdOper (en clair, tout ce qu’il y a entre 2 opérateurs « && » ou « || »). Dans cette chaine de caractère, le parseur recherche les redirecteurs « | », « > », « >> », « < », « << ». Puis il construit petit à petit une liste chaînée de CmdRedir reliés les uns avec les autres. Chaque CmdRedir dispose d’un certain nombre d’informations dont le type de redirecteur qui le relie au CmdRedir suivant.

Le parseur CmdArgs est plus simple que celui des CmdOper ou CmdRedir. Il parcourt une chaîne de caractère qui correspond à un CmdRedir (en clair, tout ce qu’il y a entre 2 redirecteurs « | », « > », « >> », « < », « << »). Dans cette chaîne de caractères, le parseur recherche des espaces afin de générer une liste d’arguments. Attention, les simples ou doubles quotes sont bien gérés. Un espace dans un argument entre double quote ne donnera pas lieu à 2 arguments mais bien à un seul.

## Exemple d'interprétation
![Principe](media/principe_cmdoper_redir_args.png?raw=true "Principe des CmdOper, CmdRedir et CmdArgs")

## Parseur CmdAlias
Enfin, le parseur CmdAlias est très simple. Il analyse une chaîne à la recherche d’une définition d’alias, sur le modèle suivant : nom_alias='commande argument1 argument2 ...'
