float calc(float a, float b)
{
    Print("# creating refcounted object\n");
    Horse@ ref1 = Horse('Jurasek');
    
    Print("# adding ref\n");
    Horse@ ref2 = ref1;
    
    Print("# removing ref\n");
    @ref2 = null;
    
    Print("# deleting refcounted object\n");
    @ref1 = null;
    
    Print("# creating ref horse\n");
    Horse@ ho = Horse("Semik");
    
    Print("# putting ref horse to stable via implicitly-constructed Handle object\n");
    PutToStableViaHandle(ho);    

    Print("# Erase local horse ref\n");
    @ho = null;
    
    Print("# Creating horse with RCOH handle\n");
    RefCountingObjectHandle@ rcoh_horse = Horse('Spirit');
    
    Print("# Initializing horse ref from RCOH handle\n");
    Horse@ exRcoh_horse = cast<Horse>(rcoh_horse);
    
    Print("# putting ref horse to stable via explicit Handle object\n");
    PutToStableViaHandle(rcoh_horse);
    
    Print("# Clearing the RCOH handle\n");
    @rcoh_horse = null;
    
    Print("# Clearing the local ref\n");
    @exRcoh_horse = null;  

    Print("# Assign RCOH handle from stable, via explicit Handle object");
    @rcoh_horse = FetchFromStableViaHandle();
    
    Print("# Dump horse from stable, via null Handle object\n");
    PutToStableViaHandle(null);
    
    Print("# Make sure the RCOH handle from stable is in consistent state\n");
    @exRcoh_horse = cast<Horse>(rcoh_horse);
    
    Print("# Clearing the RCOH handle\n");
    @rcoh_horse = null;

    Print("# putting unreferenced horse to stable\n");
    PutToStableViaHandle(Horse("Rossinante"));
    
    Print("# putting another unref horse to stable\n");
    PutToStableViaHandle(Horse("Jolly Jumper")); 

    Print("# fetching horse back from stable\n");
    @ho = cast<Horse>(FetchFromStableViaHandle());
    
    Print("# Dump horse from stable\n");
    PutToStableViaHandle(null);
    
    Print("# Erase local horse ref\n");
    @ho = null;    

    // Print the value that we received
    Print("# Received: " + a + ", " + b + "\n");
    
    // Print the current system time
    Print("# System has been running for " + GetSystemTime()/1000.0 + " seconds\n");
    
    // Do the calculation and return the value to the application
    return a * b;
}
