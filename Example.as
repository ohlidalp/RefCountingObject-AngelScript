void NativePtrTest()
{
    Print("# creating refcounted object\n");
    Horse@ ref1 = Horse(); // 'Jurasek'
    
    Print("# adding ref\n");
    Horse@ ref2 = ref1;
    
    Print("# create null ref\n");
    Horse@ ref3 = null;
    
    Print("# assign object to null ref\n");
    @ref3 = ref1;
    
    Print("# removing 2 refs\n");
    @ref3 = null;
    @ref2 = null;
    
    Print("# 2 ref goes out of scope - object will be deleted\n");
    // ref1
}

void CustomizedPtrTest()
{
    Print("# creating refcounted object using customized handle\n");
    HorsePtr@ ref1 = Horse(); // 'Shadowfax'
    //ref1.Neigh(); // Ptrs don't work this way
    
    Print("# adding ref using customized handle\n");
    HorsePtr@ ref2 = ref1;
    
    Print("# Test equality on customized handles\n");
    Print("`(ref2 == ref1)`: " + (ref2 == ref1) + "\n");
    Print("`(@ref2 == @ref1)`: " + (@ref2 == @ref1) + "\n");
    
    Print("# create null customized handle\n");
    HorsePtr@ ref3 = null;
    
    Print("# Test equality on null/notnull customized handles\n");
    Print("`(ref2 == ref3)`: " + (ref2 == ref3) + "\n");
    Print("`(@ref2 == @ref3)`: " + (@ref2 == @ref3) + "\n");    
    
    Print("# assign object to null customized handle\n");
    @ref3 = ref1;
    
    Print("# removing 2 refs using customized handle\n");
    @ref3 = null;
    @ref2 = null;
    
    Print("# adding 2 refs using customized handle and '=' chain\n");
    @ref3 = @ref2 = @ref1;
    
    Print("# create and use 2 anonymous native handles\n");
    Horse(ref2).Neigh();
    Horse(ref3).Neigh();
    
    Print("# removing 2 refs using customized handle\n");
    @ref3 = null;
    @ref2 = null;    
    
    Print("# assign object to new native handle\n");
    Horse@ nh1 = ref1;
    nh1.Neigh();
    
    Print("# Test equality of customized and native handles\n");
    Print("`(nh1 == ref1)`: " + (nh1 == ref1) + "\n");
    Print("`(@nh1 == @ref1)`: " + (@nh1 == @ref1) + "\n");     
    
    Print("# assign object to native null handle\n");
    Horse@ nh2 = null;
    @nh2 = ref1;
    nh2.Neigh();
    
    Print("# clear native handle\n");
    @nh1 = null;
    
    Print("# assign object from native handle to null customized handle\n");
    @ref2 = @nh2;
    
    Print("# clear native handle\n");
    @nh2 = null;   

    Print("# removing ref using customized handle\n");
    @ref2 = null;
    
    Print("# 1 ref goes out of scope - object will be deleted\n");
    // ref1
}

void AppInterfaceNativePtrTest()
{
    Print("# creating ref horse\n");
    Horse@ ho = Horse(); // "Semik"
    
    Print("# putting ref horse to stable via implicitly-constructed generic handle\n");
    PutToStable(ho);    

    Print("# Erase local horse ref\n");
    @ho = null;
    
    Print("# putting unreferenced horse to stable\n");
    PutToStable(Horse()); // "Rossinante"

    Print("# fetching horse back from stable\n");
    @ho = FetchFromStable();
    
    Print("# Dump horse from stable\n");
    PutToStable(null);
    
    Print("# Erase local horse ref\n");
    @ho = null;    
}

void AppInterfaceCustomizedPtrTest()
{
    Print("# creating ref horse using customized handle\n");
    HorsePtr@ ho = Horse(); // "Jolly Jumper"
    
    Print("# putting ref horse to stable via explicit customized handle\n");
    PutToStable(ho);    

    Print("# Erase local horse ref\n");
    @ho = null;
    
    Print("# putting unreferenced horse to stable by explicitly constructing customized handle\n");
    PutToStable(HorsePtr(Horse())); // "Fru-Fru"

    Print("# fetching horse back from stable to customized handle\n");
    @ho = FetchFromStable();
    
    Print("# impicitly cast customized handle (from stable) to native handle\n");
    Horse@ native = @ho;
    native.Neigh();
    
    Print("# Erase native handle\n");
    @native = null;
    
    Print("# Dump horse from stable\n");
    PutToStable(null);
    
    Print("# 1 ref goes out of scope - object will be deleted\n");
    // ho
}

void ExampleAngelScript()
{
    Print("##  BEGIN native handle test\n");
    NativePtrTest();
    Print("##  END native handle test\n");
    
    Print("##  BEGIN Customized handle test\n");
    CustomizedPtrTest();
    Print("##  END Customized handle test\n");
    
    Print("##  BEGIN app interface + native handle test\n");
    AppInterfaceNativePtrTest();
    Print("##  END app interface + native handle test\n");
    
    Print("##  BEGIN app interface + Customized handle test\n");
    AppInterfaceCustomizedPtrTest();
    Print("##  END app interface + Customized handle test\n");
    
     
    Print("# Create parrot\n");
    Parrot@ parr = Parrot();
    parr.Chirp();
    
    Print("# Test aviary\n");
    PutToAviary(parr);
    
    Print("# Erase parrot ref\n");
    @parr = null;
    
    Print("# Dump parrot from aviary\n");
    PutToAviary(null);
}
