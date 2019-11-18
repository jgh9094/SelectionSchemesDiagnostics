
# Ideas for Diagnostics

<!-- TOC -->

- [Ideas for Diagnostics](#ideas-for-diagnostics)
  - [Diagnostics](#diagnostics)
    - [Exploitation Diagnostic](#exploitation-diagnostic)
    - [Structured Exploitation Diagnostic - Structured K Traits](#structured-exploitation-diagnostic---structured-k-traits)
    - [Ecology Diagnostic - Contradictory K Traits](#ecology-diagnostic---contradictory-k-traits)
    - [Ecology Diagnostic](#ecology-diagnostic)
    - [Specialist Diagnostic](#specialist-diagnostic)
    - [Hints Diagnostic](#hints-diagnostic)
    - [Bias diagnostic](#bias-diagnostic)
    - [Deceptive landscape](#deceptive-landscape)
    - [Overfitting diagnostic - Noise](#overfitting-diagnostic---noise)
    - [Overfitting diagnostic - Smoothness](#overfitting-diagnostic---smoothness)
    - [Exploration diagnostic](#exploration-diagnostic)
    - [Pareto front diagnostic **MAYBE**](#pareto-front-diagnostic-maybe)

<!-- /TOC -->

## Diagnostics

- **X is the target value**
- **K is the number of org internal traits**

### Exploitation Diagnostic

- Purpose:
  - This will allow us to see if selection schemes can reach the target objective, even though there are multiple ways to get there.
  - Orgs will try to exploit a specific internal value.
- Literature:
  - Exploration and Exploitation in Evolutionary Algorithms: A Survey
- ORG Structure:
  - Organisms are length-K vectors of doubles.
  - K internal traits (vector of k traits).
    - [v<sub>1</sub>, v<sub>2</sub>, ..., v<sub>k</sub>]
  - Mutations can increase or decrease a specific K value.
- Problem:
  - The environment is represented as K targets, each target _i_ corresponding to
    position _i_ in organism genomes.
  - Goal: to maximize fitness, each position in a genome should be as close
    as possible to the associated target _i_.
  - fitness(org, i) = |X - v<sub>i</sub>|, where v<sub>i</sub> is the internal value at position i.
- Analysis:
  - How close to X can orgs get?
  - Could add neutral space around targets?
  - Phenotypic diversity
  - Phylogenetic tree
  - Solution count
    - A solution is an org that has at least 1 internal value that has reached X
- Extension:
  - K numbers, all K internal traits need to be X
  - As there are more and more numbers do you lose precision?
  - Add neutral space around targets

### Structured Exploitation Diagnostic - Structured K Traits

- Purpose:
  - Some problems are structured in such a way where they require approach
    building block when trying to reach a solution.
  - This provides more of a relationship between the orgs internal traits.
  - We want the organisms to optimize there traits from start (v<sub>1</sub>) to end (v<sub>k</sub>)
- Literature:
  - Exploration and Exploitation in Evolutionary Algorithms: A Survey
- ORG Structure:
  - Organisms are length-K vectors of doubles.
  - K internal traits (vector of k traits)
    - [v<sub>1</sub>, v<sub>2</sub>, ..., v<sub>k</sub>]
  - Mutations can increase or decrease a specific k value
- Problem:
  - The environment is represented as K targets, each target _i_ corresponding to
    position _i_ in organism genomes.
  - Evaluation:
    - Starting from the beginning of an organism's genome, the subsequent value
      is evaluated only if it is greater than or equal to the first value.
  - Evolution should optimize the first value, then the second, then third,
    _etc._
  - fitness(org) = [f<sub>1</sub>, f<sub>2</sub>, f<sub>3</sub>, ..., f<sub>K</sub>]
    - for all f<sub>i</sub> in fitness(org):
      - f<sub>1</sub> = |X - v<sub>1</sub>|
      - f<sub>j</sub> (for all j in [2,K]):
        - if f<sub>j-1</sub> <= |X - v<sub>j</sub>|, -> f<sub>j</sub> = |X - v<sub>j</sub>|
        - else -> f<sub> j </sub> = MAX_ERROR
- Analysis:
  - See how long the streak of successful internal traits grows
  - Phenotypic diversity
  - Phylogenetic tree
  - Solution count

### Ecology Diagnostic - Contradictory K Traits

- Purpose:
  - If any individual organism can only be selected for one of the K traits,
    how many distinct niches can be filled at any point in time?
- Literature:
  - STILL LOOKING ._.
- ORG Structure:
  - Organisms are length-K vectors of doubles.
  - K internal traits (vector of k traits)
    - [v<sub>1</sub>, v<sub>2</sub>, ..., v<sub>k</sub>]
  - Mutations can increase or decrease a specific k value
- Problem:
  - t* = min(|t-T|), trait that gives us the minimum error
  - Fitness contribution for each trait:
    - for i in [1,K]:
      - fitness[i] = |(t*-t<sub>i</sub>) - T|
      - if t* = t<sub>i</sub>:
        - fitness[i] = |t* - T|
- Analysis:
  - Solution count
  - Phenotypic diversity
  - Phylogenetic tree

### Ecology Diagnostic

- Purpose:
  - **default** ecology diagnostic
- Literature:
  - STILL LOOKING ._.
- ORG Structure:
  - Organisms are length-K vectors of doubles.
  - Mutations can increase or decrease a specific K value
- Problem:
  - Same as harsh ecology diagnostic above, but no trait can take on a score
    below zero.
    - i.e., organisms only receive fitness from their maximum trait value; all
      other traits are given a score of zero (instead of a negative value)
- Analysis:
  - Solution count
    - **What does a solution look like?**
  - Phenotypic diversity
  - Phylogenetic tree

### Specialist Diagnostic

- Purpose:
  - How does the proposed selection scheme deal with specialist preservation?
- Literature:
  - STILL LOOKING ._.
- ORG Structure:
  - Organisms are length-K vectors of doubles (each double representing a trait).
  - K internal traits (vector of k traits)
    - [v<sub>1</sub>, v<sub>2</sub>, ..., v<sub>k</sub>]
  - One of the K internal traits is a specialist trait (v<sub>k</sub>)
  - Mutations can increase or decrease a specific k value
- Problem:
  - Variant on the ecology diagnostic.
  - Score on non-specialist traits is value in trait minus the score in the
    specialist trait.
  - Score on specialist trait is just how close it is to the target for that
    trait position.
- Analysis:
  - Solution count
    - **What does a solution look like?**
  - Phenotypic diversity
  - Phylogenetic tree

### Hints Diagnostic

- Purpose:
  - When dealing with a huge testing space, it can be difficult to know if all test cases are correct.
  - How does a selection scheme contend with this challenge.
- Literature:
  - STILL LOOKING ._.
  - Emily's GPTP paper looking at Eco-EA, lexicase, etc selection schemes in
    context of bad hints.
- ORG Structure:
  - Organisms are length-K vectors of doubles (each double representing a trait).
  - K internal traits (vector of k traits)
    - [v<sub>1</sub>, v<sub>2</sub>, ..., v<sub>k</sub>]
  - Mutations can increase or decrease a specific k value
- Problem:
  - Extension of exploitation diagnostic.
  - An organism should be be trying to get each of its K internal traits to
    a target.
  - We will add hints (good, bad, or neutral) to our fitness evaluation.
  - Good hints:
    - Aggregate fitness (sum of fitness scores across all traits) as a 'hint'
      trait.
  - Bad hints:
    - 'hint' trait: Sum of error across all traits. Maximizing this contradicts
      global fitness maximum.
- Analysis:
  - How much does a single good hint vs single bad hint change outcomes?
  - Solution count
    - All internal traits have reached X.
  - Phenotypic diversity
  - Phylogenetic tree

### Bias diagnostic

- Purpose:
  - When dealing with a huge testing space, it can be difficult to know what test cases are redundant.
  - How does a selection scheme contend with this challenge.
- Literature:
  - STILL LOOKING ._.
- ORG Structure:
  - Organisms are length-K vectors of doubles (each double representing a trait).
  - K internal traits (vector of k traits)
    - [v<sub>1</sub>, v<sub>2</sub>, ..., v<sub>k</sub>]
  - Mutations can increase or decrease a specific k value
- Problem:
  - We will choose a particular test case to bias fitness toward.
  - Let _n_ be the number of times we duplicate a particular test case in the
    testing set.
  - _n_ specifies how biased fitness scores are to the chosen test case.
- Analysis:
  - How much does a biasing change outcomes?
  - Solution count
    - All internal traits reach X
  - Phenotypic diversity
  - Phylogenetic tree

### Deceptive landscape

- Purpose:
  - When dealing with certain problems, the landscape may be deceptive to an organisms exploration.
  - How does a selection scheme contend with this challenge.
- Literature:
  - STILL LOOKING ._.
- ORG Structure:
  - Organisms are length-K vectors of doubles (each double representing a trait).
  - K internal traits (vector of k traits).
    - [v<sub>1</sub>, v<sub>2</sub>, ..., v<sub>k</sub>]
  - Mutations can increase or decrease a specific k value
- Problem:
  - Version 1
    - Given K and X beforehand, can organisms reach X for all K internal traits?
    - Fitness(org, X, v<sub>i</sub>) = tens place - ones place
      - e.g., let Xi = 100, trait _i_ = 78. Fitness for trait _i_ = 70 - 8 = 62
    - If an organism's internal value goes over 100, will do the same process as above
- Analysis:
  - How much does a deceptive landscape affect solutions attempting to reach objective
  - Solution count
  - Phenotypic diversity
  - Phylogenetic tree
  - Exploitation + sawtooth effect

### Overfitting diagnostic - Noise

- Purpose:
  - When dealing with data, there may be noise associated with it
  - How does a selection scheme contend with this challenge
- Literature:
  - Genetic Algorithms, Tournament Selection, and the Effects of Noise
  - Ton of ML literature on this subject
- ORG Structure:
  - Organisms are length-K vectors of doubles (each double representing a trait).
  - K internal traits (vector of k traits)
    - [v<sub>1</sub>, v<sub>2</sub>, ..., v<sub>k</sub>]
  - Mutations can increase or decrease a specific k value
- Problem:
  - Instead of having 1 test for each trait we will have _n_ tests for each
    trait where _n_ specifies the number of noisy tests we use.
    - In total, there will be K * _n_ total tests.
  - Tests are generated from Normal(mean=X, sigma).
  - Do organisms overfit a 'noisy' test, or identify the mean?
- Analysis:
  - How much does noise affect solutions attempting to reach objective?
  - Solution count
    - All internal traits reach X
  - Phenotypic diversity
  - Phylogenetic tree

### Overfitting diagnostic - Smoothness

- Purpose:
  - Please help...
- Literature:
  - Ton of ML literature on this subject
- ORG Structure:
  - Organisms are length-K vectors of doubles (each double representing a trait).
  - K internal traits (vector of k traits)
    - [v<sub>1</sub>, v<sub>2</sub>, ..., v<sub>k</sub>]
  - Mutations can increase or decrease a specific k value
  - No instructions required, since org is essentially a bit string
- Problem:
  - We will define a function before hand, f(x) = x^2 for example
  - Each internal value has a unique value associated with it
  - [v<sub>1</sub>=1, v<sub>2</sub>=2, ..., v<sub>k</sub>=k]
  - f(v<sub>1</sub>) = f(1) = 1, f(v<sub>2</sub>) = f(2) = 4, ..., f(k) = f(k) = k^2
  - fitness(org) = sum(f(v<sub>i</sub>) - g(v<sub>i</sub>)), for all internal traits where f is the ground truth and g is the solution's response
- Analysis:
  - How much does a multiobjective problem affect solutions attempting to reach objective.
  - Solution count
    - All internal traits must match the ground truth
  - Phenotypic diversity
  - Phylogenetic tree

### Exploration diagnostic

- Only the highest trait counts & sequentially from there, going forward any traits that are lower than it
- Purpose:
  - If there are many good places to go from here, can I find the right path?
- Literature:
  - STILL LOOKING ._.
- ORG Structure:
  - - Organisms are length-K vectors of doubles (each double representing a trait).
  - K internal traits (vector of k traits)
    - [v<sub>1</sub>, v<sub>2</sub>, ..., v<sub>k</sub>]
  - Mutations can increase or decrease a specific k value
  - No instructions required, since org is essentially a bit string
- Problem:
  - fitness(org, K, X) = |X - v<sub>i</sub>| + ... + |X - v<sub>k</sub>|
    - v<sub>i</sub> is the internal value that minimizes |X - v|
  - We then sum all internal traits to the right
- Analysis:
  - How much does a problem structure affect solutions attempting to reach objective.
  - Solution count
  - Phenotypic diversity
  - Phylogenetic tree
- Expectation:
  - Hierarchical fair competition should perform well here.
  - Selection schemes that select for 'young' lineages should also perform
    well here.

### Pareto front diagnostic **MAYBE**

- explicit trade-offs
- overall target, every time there's a mutation => equal and opposite mutation on another trait
- how many combinations can the selection scheme look at?
- lots possible!
