# clear any data
rm(list=ls())
# import the libs
library(readr)
library(tidyr)
library(ggplot2)
library(plyr)
library(reshape2)
library(cowplot)
library(tidyverse)
library(dplyr)
source("https://gist.githubusercontent.com/benmarwick/2a1bb0133ff568cbe28d/raw/fb53bd97121f7f9ce947837ef1a4c65a73bffb3f/geom_flat_violin.R")

# set working dir
setwd("C:/Users/josex/Desktop/Research/Ofria/Project/SelectionDiagnostics/SelectionSchemesDiagnostics/Visuals")

# Let us begin
DATA_DIR="../Scrape"
DATA_FILES=c("lex_avg_err_pop_10.csv", "lex_avg_err_pop_100.csv", "lex_avg_err_pop_500.csv", "lex_avg_err_pop_700.csv", "lex_avg_err_pop_1000.csv")
REPS=100
INTER=c(3,7,12)

ERR = c()
GEN = c()
POP = c()
X = c(rep(seq(1,100), length(INTER)))

for(f in 1:length(DATA_FILES)) {
  data = read.csv(paste(DATA_DIR, DATA_FILES[f], sep="/"), header=FALSE)
  #iterate through the rows
  for(i in INTER) {
    row = as.numeric(as.vector(unlist(data[i,])))
    ERR = c(ERR, row[c(3:length(row))])
    GEN = c(GEN, rep(row[c(1)], REPS))
    POP = c(POP, as.character(rep(row[c(2)], REPS)))
  }
}

# Create data frame
data = data.frame(x = X, gen = GEN, pop=POP, err=ERR)

# Plot the results!
ggplot(data=data, aes(x=pop, y=err, fill=pop)) +
  geom_flat_violin(position = position_nudge(x = .2, y = 0), alpha = 1.0) +
  geom_point(aes(y = err, color = pop), 
             position = position_jitter(width = .15), size = .5, alpha = 1.0) +
  geom_boxplot(width = .1, outlier.shape = NA, alpha = 0.5) +
  ylab("Average Population Error") +
  xlab("Population Sizes") +
  scale_x_discrete(limits=c("10", "100", "500", "700", "1000"))+
  ggtitle("Lexicase Population Error w/ Different Pop Sizes") +
  facet_grid(rows = vars(gen))
  
  







