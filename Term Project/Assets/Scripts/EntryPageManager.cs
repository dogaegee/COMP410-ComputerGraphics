using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class EntryPageManager : MonoBehaviour
{

    public AudioClip buttonSound;
    private AudioSource buttonAudioSource;

    // Start is called before the first frame update
    public void playGame()
    {
        buttonAudioSource = gameObject.AddComponent<AudioSource>(); 
        buttonAudioSource.PlayOneShot(buttonSound);

        StartCoroutine(LoadNextSceneAfterDelay(0.6f));
    }

    private IEnumerator LoadNextSceneAfterDelay(float delay)
    {
        yield return new WaitForSeconds(delay);
        SceneManager.LoadScene(SceneManager.GetActiveScene().buildIndex + 1);
    }


    public void quitGame(){
        Application.Quit();
    }
}
