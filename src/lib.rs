#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!("./bindings.rs");

    
#[test]
fn example1() {
    unsafe {
        // Building the EncryptionParameters object
        let mut ep: seal_EncryptionParameters = Default::default();
        seal_EncryptionParameters_EncryptionParameters(&mut ep, seal_scheme_type_BFV);
        
        // Construct the context
        let mut ctx = seal_SEALContext_Create(&ep, false);
    }
}
