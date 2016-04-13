#Find living artists who are considered feminists and were born during WW2
#give their name, birthyear and, if they have a website, give that too. 
PREFIX ont: <http://dbpedia.org/ontology/>
PREFIX foaf:  <http://xmlns.com/foaf/0.1/>
SELECT ?Person ?Homepage bif:year(?date) AS ?year
WHERE 
{
    ?Person a dbo:Artist ;
       a yago:LivingPeople ;
       a yago:Feminist110084635 . 
    ?Person ont:birthDate ?date .
FILTER
(
    !bif:isNull(xsd:date(?date)) 
)
FILTER
(
    ?date > "1939-09-01"^^xsd:dateTime
&&
    ?date < "1945-09-02"^^xsd:dateTime

)
OPTIONAL 
{
?Person  foaf:homepage ?Homepage
}
}
LIMIT 100