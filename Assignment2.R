##Assignment 2

# Download the CGDS package 
source("http://www.bioconductor.org/biocLite.R")
biocLite("cgdsr")
library("cgdsr")

# Create CGDS object
mycgds <- CGDS("http://www.cbioportal.org/public-portal/")

# Get the list of cancer studies at server
mystud <- getCancerStudies(mycgds)

# Find thca_tcga from available case lists using grep
grep("thca_tcga",mystud[,1])

# Pick the correct result
mystud[c(86,116),1]

# View available case lists, find the name of the correct id
View(getCaseLists(mycgds,mystud[116,1]))

# View available genetic profiles, find the name of the correct id
View(getGeneticProfiles(mycgds,"thca_tcga")[1,1])

# View data slices for specified list of genes, genetic profile and case list
View(getProfileData(mycgds, "BRAF", "thca_tcga_mutations", "thca_tcga_all"))

# Store data slices in a variable
res <-getProfileData(mycgds,"BRAF","thca_tcga_mutations","thca_tcga_all")

# Generate a summary for more convenient viewing
resdf<-as.data.frame(summary(res$BRAF))

# Fix the column name
colnames(resdf) <-"BRAF mutations"

View(resdf)