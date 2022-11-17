package com.trivi12.estacionamientoutn.view

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.Menu
import android.view.MenuItem
import com.trivi12.estacionamientoutn.R
import com.trivi12.estacionamientoutn.databinding.ActivityMainBinding
import com.trivi12.estacionamientoutn.model.ParkingLot
import com.trivi12.estacionamientoutn.viewmodel.MainViewModel

class MainActivity : AppCompatActivity() {
    private lateinit var binding : ActivityMainBinding
    private var viewModel: MainViewModel? = null
    private lateinit var listParkingLot: ArrayList<ParkingLot>
    private var menu :Menu?=null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        viewModel = MainViewModel()
        viewModel!!.listParkingLots.observe(this){list ->
            listParkingLot = list as ArrayList<ParkingLot>
            setupView()
        }

        viewModel!!.getStatusParking()
    }

    override fun onCreateOptionsMenu(menu: Menu?): Boolean {
        menuInflater.inflate(R.menu.refresh_menu, menu)
        this.menu = menu
        return super.onCreateOptionsMenu(menu)
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        when (item.itemId){
            R.id.refresh->{
                viewModel!!.getStatusParking()
            }
        }
        return super.onOptionsItemSelected(item)
    }

    private fun setupView(){
        for (parkingLot in listParkingLot){
            if (parkingLot.title == "Nodo 1"){
                if (parkingLot.taken){
                    binding.ivParkingLots1.root.background = getDrawable(R.drawable.backgrouns_taken)
                }else{
                    binding.ivParkingLots1.root.background = getDrawable(R.drawable.background_free)
                }
            }
        }
    }
}