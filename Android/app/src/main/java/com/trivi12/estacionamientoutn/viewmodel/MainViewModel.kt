package com.trivi12.estacionamientoutn.viewmodel

import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.google.firebase.database.DatabaseReference
import com.google.firebase.database.FirebaseDatabase
import com.trivi12.estacionamientoutn.model.ParkingLot
import kotlinx.coroutines.launch

class MainViewModel : ViewModel() {
    private lateinit var db : DatabaseReference
    val listParkingLots = MutableLiveData<ArrayList<ParkingLot>>()

    fun getStatusParking(){
        val parkingLots = ArrayList<ParkingLot>()
        viewModelScope.launch {
            db = FirebaseDatabase.getInstance().getReference("Nodos")
            db.get().addOnSuccessListener { documents ->
                for(data in documents.children){

                    val taken = data.child("Actual").value as Boolean
                    val title = data.key

                    val parkingLot = ParkingLot(taken, title.toString())
                    parkingLots.add(parkingLot)
                }
                listParkingLots.value = ArrayList(parkingLots)
            }
        }
    }
}