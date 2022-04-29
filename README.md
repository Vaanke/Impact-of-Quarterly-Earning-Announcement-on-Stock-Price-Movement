# 6883Project

eod historicaldata token: 61ad392951f604.64641793

## 12/10/2021 update
- DataReader is ready. Retrieve all price data in 2 min (N=70)
- changed the data structure price data to imrpove efficieny. price dates and price value are stored in two vectors. added startingdate & endgingdate variable
- updated related member functions in class Stock. added a default constructor and a copy constructor

## 12/11/2021 update
- Group and Stock Module are ready. 
- These two modules for implementing bootstrap algo to calculate AAR, CAAR, AAR_STD, CAAR_STD (stored as private data in Group Class).
- Update main funcitons for these two modules

## 12/16/2021 update
- Display module is ready
- Menu is ready. DataReader is slightly changed to accommodate changes in main function
- All modules are integrated and tested.