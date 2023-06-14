using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;
public class SubtitleManager : MonoBehaviour
{
    
    [SerializeField] private TextMeshProUGUI itemInfoText;
    [SerializeField] [TextArea] private string[] itemInfo;
    [SerializeField] private float textSpeed = 0.01f;
    public int currentDisplayingText = 0;
    private static SubtitleManager instance;
    public static SubtitleManager Instance
    {
        get { return instance; }
    }

    public GameObject subtitlePanel;

    private void Awake()
    {
        DisablePanel();
        if (instance != null && instance != this)
        {
            Destroy(this.gameObject);
        }
        else
        {
            instance = this;
            DontDestroyOnLoad(this.gameObject);
        }
    }

    public void EnablePanel()
    {
        subtitlePanel.SetActive(true);
        StartCoroutine(AnimateText());
    }

    public void DisablePanel()
    {
        subtitlePanel.SetActive(false);
    }

    IEnumerator AnimateText(){
        if (ScoreManager.Instance.GetScore() > 0)
        {
            for(int i = 0; i < itemInfo[currentDisplayingText].Length + 1; i++){
                itemInfoText.text = itemInfo[currentDisplayingText].Substring(0,i);
                yield return new WaitForSeconds(textSpeed);
            }
        }
    }
}


