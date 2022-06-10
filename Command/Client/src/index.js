import React from 'react';
import ReactDOM from "react-dom/client";
import Map from './map';
import Movement from './Movement';
import Grid from '@material-ui/core/Grid';

const root = ReactDOM.createRoot(document.getElementById("root"));
root.render(
    <Grid container spacing={2}> 
        <Grid item xs={12} sm={6}>
        <Movement />
        </Grid>
        <Grid item xs={12} sm={6}>
        <Map />
        </Grid>
    </Grid>
);

//addalienmanual
//delaliens